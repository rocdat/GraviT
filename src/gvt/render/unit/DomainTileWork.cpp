/* =======================================================================================
   This file is released as part of GraviT - scalable, platform independent ray
   tracing
   tacc.github.io/GraviT

   Copyright 2013-2015 Texas Advanced Computing Center, The University of Texas
   at Austin
   All rights reserved.

   Licensed under the BSD 3-Clause License, (the "License"); you may not use
   this file
   except in compliance with the License.
   A copy of the License is included with this software in the file LICENSE.
   If your copy does not contain the License, you may obtain a copy of the
   License at:

       http://opensource.org/licenses/BSD-3-Clause

   Unless required by applicable law or agreed to in writing, software
   distributed under
   the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY
   KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under
   limitations under the License.

   GraviT is funded in part by the US National Science Foundation under awards
   ACI-1339863,
   ACI-1339881 and ACI-1339840
   =======================================================================================
   */

//
// DomainTileWork.cpp
//

#include "gvt/render/unit/DomainTileWork.h"
#include "gvt/render/unit/DoneTestWork.h"
#include "gvt/render/unit/PixelGatherWork.h"
#include "gvt/render/unit/RayWork.h"
#include "gvt/render/unit/MpiRenderer.h"

#ifdef GVT_RENDER_ADAPTER_EMBREE
#include <gvt/render/adapter/embree/Wrapper.h>
#endif

#ifdef GVT_RENDER_ADAPTER_MANTA
#include <gvt/render/adapter/manta/Wrapper.h>
#endif

#ifdef GVT_RENDER_ADAPTER_OPTIX
#include <gvt/render/adapter/optix/Wrapper.h>
#endif

#if defined(GVT_RENDER_ADAPTER_OPTIX) && defined(GVT_RENDER_ADAPTER_EMBREE)
#include <gvt/render/adapter/heterogeneous/Wrapper.h>
#endif

#include <boost/timer/timer.hpp>

using namespace gvt::core::mpi;
using namespace gvt::render::unit;
using namespace gvt::render::actor;

#define DEBUG_DOMAIN_TILE_WORK

WORK_CLASS(DomainTileWork)

Work* DomainTileWork::Deserialize(size_t size, unsigned char* serialized) {
  if (size != (4 * sizeof(int))) {
    std::cerr << "Test deserializer ctor with size != 4 * sizeof(int)\n";
    exit(1);
  }

  unsigned char* buf = serialized;
  DomainTileWork* tileWork = new DomainTileWork;

  tileWork->startX = *reinterpret_cast<int*>(buf); buf += sizeof(int);
  tileWork->startY = *reinterpret_cast<int*>(buf); buf += sizeof(int);
  tileWork->width  = *reinterpret_cast<int*>(buf); buf += sizeof(int);
  tileWork->height = *reinterpret_cast<int*>(buf); buf += sizeof(int);

  return tileWork;
}

void DomainTileWork::setupAction() {
  TileWork::setupAction();
  myRank = Application::GetApplication()->GetRank();
  doneTestLock = renderer->getDoneTestLock();
  doneTestCondition = renderer->getDoneTestCondition();
}

bool DomainTileWork::Action() {

#ifdef DEBUG_TILE_WORK
  printf("Rank %d: start processing tile(%d, %d, %d, %d)\n",
          Application::GetApplication()->GetRank(),
          startX, startY, width, height);
#endif

  setupAction();

  RayVector rays;
  generatePrimaryRays(rays);

  traceRays(rays);

  return false;
}

void DomainTileWork::filterRaysLocally(RayVector& rays) {
  auto nullNode = gvt::core::DBNodeH(); // temporary workaround until
                                        // shuffleRays is fully replaced
  std::map<int, RayVector>& queue = *rayQueue; 
  shuffleRays(rays, nullNode);
  for (auto e : queue) {
    if (renderer->getInstanceOwner(e.first) != myRank) {
      GVT_DEBUG(DBG_ALWAYS, " rank[" << myRank
                                     << "] FILTERRAYS: removing queue "
                                     << e.first);
      queue[e.first].clear();
    }
  }
}

void DomainTileWork::traceRays(RayVector& rays) {

#ifdef DEBUG_DOMAIN_TILE_WORK
  printf("Rank %d: tracing rays using domain scheduler\n", myRank);
#endif

  std::map<int, RayVector>& queue = *rayQueue; 

  boost::timer::cpu_timer t_sched;
  t_sched.start();
  boost::timer::cpu_timer t_trace;
  GVT_DEBUG(DBG_ALWAYS,
            "domain scheduler: starting, num rays: " << rays.size());
  int adapterType =
      gvt::core::variant_toInteger(root["Schedule"]["adapter"].value());
  long domain_counter = 0;

  // FindNeighbors();

  // sort rays into queues
  // note: right now throws away rays that do not hit any domain owned by the
  // current
  // rank
#ifdef GVT_USE_MPE
  MPE_Log_event(localrayfilterstart, 0, NULL);
#endif
  filterRaysLocally(rays);
#ifdef GVT_USE_MPE
  MPE_Log_event(localrayfilterend, 0, NULL);
#endif

  GVT_DEBUG(DBG_LOW, "tracing rays");
  // process domains until all rays are terminated
  bool all_done = false;
  // std::set<int> doms_to_send;
  int lastInstance = -1;
  // gvt::render::data::domain::AbstractDomain* dom = NULL;
  gvt::render::actor::RayVector moved_rays;
  moved_rays.reserve(1000);
  int instTarget = -1;
  size_t instTargetCount = 0;
  gvt::render::Adapter *adapter = 0;
  while (!all_done) {
    if (!queue.empty()) {
      // process domain assigned to this proc with most rays queued
      // if there are queues for instances that are not assigned
      // to the current rank, erase those entries
      instTarget = -1;
      instTargetCount = 0;
      std::vector<int> to_del;
      GVT_DEBUG(DBG_ALWAYS,
                "domain scheduler: selecting next instance, num queues: "
                    << queue.size());
      for (auto &q : queue) {
        const bool inRank = (renderer->getInstanceOwner(q.first) == myRank);
        if (q.second.empty() || !inRank) {
          to_del.push_back(q.first);
          continue;
        }
        if (inRank && q.second.size() > instTargetCount) {
          instTargetCount = q.second.size();
          instTarget = q.first;
        }
      }
      // erase empty queues
      for (int instId : to_del) {
        GVT_DEBUG(DBG_ALWAYS,
                  "rank[" << myRank
                          << "] DOMAINTRACER: deleting queue for instance "
                          << instId);
        queue.erase(instId);
      }
      if (instTarget == -1) {
        continue;
      }
      GVT_DEBUG(DBG_ALWAYS, "domain scheduler: next instance: "
                                << instTarget << ", rays: " << instTargetCount
                                << " [" << myRank << "]");
      // doms_to_send.clear();
      // pnav: use this to ignore domain x:        int domi=0;if (0)
      if (instTarget >= 0) {
        GVT_DEBUG(DBG_LOW, "Getting instance " << instTarget);
        // gvt::render::Adapter *adapter = 0;
        gvt::core::DBNodeH meshNode = renderer->getMeshNode(instTarget);
        if (instTarget != lastInstance) {
          // TODO: before we would free the previous domain before loading the
          // next
          // this can be replicated by deleting the adapter
          delete adapter;
          adapter = 0;
        }
        // track domains loaded
        if (instTarget != lastInstance) {
          ++domain_counter;
          lastInstance = instTarget;
          //
          // 'getAdapterFromCache' functionality
          if (!adapter) {
            GVT_DEBUG(DBG_ALWAYS, "domain scheduler: creating new adapter");
            switch (adapterType) {
#ifdef GVT_RENDER_ADAPTER_EMBREE
            case gvt::render::adapter::Embree:
              adapter =
                  new gvt::render::adapter::embree::data::EmbreeMeshAdapter(
                      meshNode);
              break;
#endif
#ifdef GVT_RENDER_ADAPTER_MANTA
            case gvt::render::adapter::Manta:
              adapter =
                  new gvt::render::adapter::manta::data::MantaMeshAdapter(
                      meshNode);
              break;
#endif
#ifdef GVT_RENDER_ADAPTER_OPTIX
            case gvt::render::adapter::Optix:
              adapter =
                  new gvt::render::adapter::optix::data::OptixMeshAdapter(
                      meshNode);
              break;
#endif
#if defined(GVT_RENDER_ADAPTER_OPTIX) && defined(GVT_RENDER_ADAPTER_EMBREE)
            case gvt::render::adapter::Heterogeneous:
              adapter = new gvt::render::adapter::heterogeneous::data::HeterogeneousMeshAdapter(
                  meshNode);
              break;
#endif
            default:
              GVT_DEBUG(DBG_SEVERE, "domain scheduler: unknown adapter type: "
                                        << adapterType);
            }
            // adapterCache[meshNode.UUID()] = adapter; // note: cache logic
            // comes later when we implement hybrid
          }
          // end 'getAdapterFromCache' concept
          //
        }
        GVT_ASSERT(adapter != nullptr, "domain scheduler: adapter not set");
        GVT_DEBUG(DBG_ALWAYS,
                  "[" << myRank
                      << "] domain scheduler: calling process queue");
        gvt::core::DBNodeH instNode = renderer->getInstanceNode(instTarget);
        {
          t_trace.resume();
          moved_rays.reserve(queue[instTarget].size() * 10);
#ifdef GVT_USE_DEBUG
          boost::timer::auto_cpu_timer t("Tracing rays in adapter: %w\n");
#endif
#ifdef GVT_USE_MPE
          MPE_Log_event(tracestart, 0, NULL);
#endif
          adapter->trace(queue[instTarget], moved_rays, instNode);
#ifdef GVT_USE_MPE
          MPE_Log_event(traceend, 0, NULL);
#endif
          queue[instTarget].clear();
          t_trace.stop();
        }

#ifdef GVT_USE_MPE
        MPE_Log_event(shufflestart, 0, NULL);
#endif
        shuffleRays(moved_rays, instNode);
#ifdef GVT_USE_MPE
        MPE_Log_event(shuffleend, 0, NULL);
#endif
        moved_rays.clear();
      }
    } // if (!queue.empty()) {

#if GVT_USE_DEBUG
    if (!queue.empty()) {
      std::cout << "[" << mpi.rank << "] Queue is not empty" << std::endl;
      for (auto q : queue) {
        std::cout << "[" << myRank << "] [" << q.first
                  << "] : " << q.second.size() << std::endl;
      }
    }
#endif

    // done with current domain, send off rays to their proper processors.
    GVT_DEBUG(DBG_ALWAYS, "Rank [ " << myRank << "]  calling SendRays");

#ifdef DEBUG_DOMAIN_TILE_WORK
    printf("Rank %d: calling sendRays queue.size()=%d\n", myRank, queue.size());
#endif
    sendRays();
    
#ifdef DEBUG_DOMAIN_TILE_WORK
    printf("Rank %d: after sendRays queue.size()=%d\n", myRank, queue.size());
#endif

    renderer->setDoneTestRunning();

    if (myRank == 0) {
      DoneTestWork work;
      work.Broadcast(true, true);
    }

    // TODO (hpark): find the way to get rid of this synchronization
    while(renderer->isDoneTestRunning());
    all_done = renderer->isAllWorkDone();
    // printf("Rank %d: done test finished (all_done=%d)\n", myRank, all_done);
  } // while (!all_done) {

  if (myRank == 0) {
    // printf("Rank %d: PixelGatherWork broadcast start\n", myRank);
    PixelGatherWork work;
    work.Broadcast(true, true);
  }
  // printf("Rank %d: domain trace finished\n", myRank);
}

void DomainTileWork::sendRays() {

  typedef std::map<int, RayVector>::iterator RayQueueIter;
  std::map<int, RayVector>& queue = *rayQueue; 

  for (RayQueueIter q = queue.begin(); q != queue.end(); ++q) {

    int domainId = q->first;
    int ownerRank = renderer->getInstanceOwner(domainId);

    RayVector& rays = q->second; 

    if (ownerRank != myRank) {
#ifdef DEBUG_DOMAIN_TILE_WORK
      printf("Rank %d: DomainTileWork::sendRays found rays to send (size=%d)\n"
              ,myRank, rays.size());
#endif
      if (rays.size() > 0) {
        RayWork work;
        work.setRays(domainId, &rays);
        work.Send(ownerRank);
      }
      queue[q->first].clear();
    }
  }
}

