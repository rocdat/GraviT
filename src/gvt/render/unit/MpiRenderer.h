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
// MpiRenderer.h
//

#ifndef GVT_RENDER_UNIT_MPI_RENDERER_H
#define GVT_RENDER_UNIT_MPI_RENDERER_H

#include "gvt/core/DatabaseNode.h"
#include "gvt/core/Math.h"
#include "gvt/core/Types.h"
#include "gvt/core/mpi/Application.h"
#include "gvt/render/data/Primitives.h"

#include "gvt/render/Types.h"
#include "gvt/render/actor/Ray.h"

#include "gvt/render/algorithm/TracerBase.h"
#include "gvt/render/unit/Voter.h"

#include <chrono>
#include <cstdint>
#include <map>
#include <pthread.h>
#include <tbb/mutex.h>
#include <vector>
#include <limits>

using namespace gvt::core::mpi;
using namespace std::chrono;

namespace gvt {
namespace render {
class Adapter;
}
}

namespace gvt {
namespace render {
namespace data {
namespace scene {
class gvtPerspectiveCamera;
class Image;
}
}
}
}

namespace gvt {
namespace render {
namespace data {
namespace accel {
class AbstractAccel;
}
}
}
}

namespace gvt {
namespace render {
namespace unit {
namespace rank {
enum RankType { Server = 0, Display = 1, FirstWorker };
}

class TileLoadBalancer;
class RayTransferWork;
class VoteWork;

struct MpiRendererOptions {
  enum SchedulerType { AsyncImage = 0, AsyncDomain, SyncImage, SyncDomain, NumSchedulers };
  virtual ~MpiRendererOptions() {}
  int schedulerType = AsyncDomain;
  int adapterType = gvt::render::adapter::Embree;
  int width = 1280;
  int height = 720;
  bool ply = false;
  int instanceCountX = 1;
  int instanceCountY = 1;
  int instanceCountZ = 1;
  int numFrames = 1;
  int numTbbThreads;
  std::string infile;
};

class Profiler {
public:
  enum Type {
    Total = 0,
    GenPrimaryRays,
    Filter,
    Schedule,
    Adapter,
    Trace,
    Shuffle,
    Send,
    Receive,
    Vote,
    Composite,
    WaitComposite,
    NumTimers
  };
  Profiler() {
    times.resize(NumTimers, 0.0);
    names.resize(NumTimers);
    names = { "Total",   "GenPrimaryRays", "Filter",  "Schedule", "Adapter",   "Trace",
              "Shuffle", "Send",           "Receive", "Vote",     "Composite", "WaitComposite" };
  }
  void update(int type, double elapsed) {
    if (type >= NumTimers) {
      printf("error: undefined profiler type %d\n", type);
      exit(1);
    }
    times[type] += elapsed;
  }

  void addRayCountProc(uint64_t n) { rays.proc += n; }
  void addRayCountSend(uint64_t n) { rays.send += n; }
  void addRayCountRecv(uint64_t n) { rays.recv += n; }

  void print(int numFrames, int numRanks) {
    // +1 to account for misc. time
    std::vector<double> sumTimes(NumTimers + 1, 0.0);
    RayCounts sumRayCounts;
    double maxRunTime = std::numeric_limits<double>::min();
    for (int p = 0; p < numRanks; ++p) {
      std::cout << "Process " << p << "\n";
      double aggregated = 0.0;
      int totalIdx = p * NumTimers + Total;
      for (int i = 0; i < NumTimers; ++i) {
        int timerIndex = p * NumTimers + i;
        if (i != Total) {
          aggregated += gtimes[timerIndex];
        } else {
          if (gtimes[timerIndex] > maxRunTime) {
            maxRunTime = gtimes[timerIndex];
          }
        }
        double avg = gtimes[timerIndex] / numFrames;
        double percent = (gtimes[timerIndex] * 100) / gtimes[totalIdx];
        std::cout << names[i] << ": " << avg << " ms (" << percent << " %)\n";
        sumTimes[i] += avg;
      }
      double misc = (gtimes[totalIdx] - aggregated) / numFrames;
      std::cout << "Misc: " << misc << " ms (" << (misc * 100) / (gtimes[totalIdx] / numFrames) << " %)\n";
      if (grays.size() == numRanks) {
        uint64_t proc = grays[p].proc / numFrames;
        uint64_t send = grays[p].send / numFrames;
        uint64_t recv = grays[p].recv / numFrames;
        std::cout << "Processed rays: " << proc << " (" << (proc * 100.0) / proc  << "%)\n";
        std::cout << "Sent rays: " << send << " (" << (send * 100.0) / proc  << "%)\n";
        std::cout << "Received rays: " << recv << " (" << (recv * 100.0) / proc  << "%)\n\n";
        sumRayCounts.proc += proc;
        sumRayCounts.send += send;
        sumRayCounts.recv += recv;
      }
    }

    // display average across all processes
    std::cout << "Processes (average)\n";
    double aggregated = 0.0;
    for (int i = 0; i < NumTimers; ++i) {
      double percent = (sumTimes[i] * 100) / sumTimes[Total];
      std::cout << names[i] << ": " << sumTimes[i] / numRanks << " ms (" << percent << " %)\n";
      if (i != Total) {
        aggregated += sumTimes[i];
      }
    }
    double misc = sumTimes[Total] - aggregated;
    std::cout << "Misc: " << misc << " ms (" << (misc * 100) / sumTimes[Total] << " %)\n";
    if (grays.size() == numRanks) {
      uint64_t proc = sumRayCounts.proc / numRanks;
      uint64_t send = sumRayCounts.send / numRanks;
      uint64_t recv = sumRayCounts.recv / numRanks;
      std::cout << "Processed rays: " << proc << " (" << (proc * 100.0) / proc << "%)\n";
      std::cout << "Sent rays: " << send << " (" << (send * 100.0) / proc << "%)\n";
      std::cout << "Received rays: " << recv << " (" << (recv * 100.0) / proc << "%)\n\n";
    }

    // max run time
    std::cout << "Max. run time (average over " << numFrames << " frames): " << maxRunTime / numFrames << " ms ("
              << numFrames / (maxRunTime * 0.001) << " fps)\n\n";
  }

private:
  friend class MpiRenderer;
  std::vector<double> times;  // times for my rank
  std::vector<double> gtimes; // times gathered from all ranks
  std::vector<std::string> names;
  struct RayCounts {
    uint64_t proc = 0;
    uint64_t send = 0;
    uint64_t recv = 0;
  };
  RayCounts rays;               // ray counts for my rank
  std::vector<RayCounts> grays; // ray counts gathered from all ranks
};

class MpiRenderer : public Application {
public:
  MpiRenderer(int *argc, char ***argv);
  virtual ~MpiRenderer();

  // for configuring database
  void parseCommandLine(int argc, char **argv);
  void createDatabase();
  void render();

private:
  // helpers
  void printUsage(const char *argv);

  // void initServer();
  void freeRender();
  void initInstanceRankMap();
  void makeObjDatabase();
  void makePlyDatabase();

private:
  MpiRendererOptions options;
  gvt::core::Vector<gvt::core::DBNodeH> instancenodes;
  gvt::core::DBNodeH root;
  std::map<int, size_t> mpiInstanceMap;
  std::map<int, gvt::render::data::primitives::Mesh *> meshRef;
  std::map<int, glm::mat4 *> instM;
  std::map<int, glm::mat4 *> instMinv;
  std::map<int, glm::mat3 *> instMinvN;
  std::vector<gvt::render::data::scene::Light *> lights;

public:
  // camera, load balancer, world bvh
  const gvt::render::data::scene::gvtPerspectiveCamera *getCamera() const { return camera; }
  TileLoadBalancer *getTileLoadBalancer() { return tileLoadBalancer; }
  gvt::render::data::accel::AbstractAccel *getAcceleration() { return acceleration; }

private:
  gvt::render::data::scene::gvtPerspectiveCamera *camera;
  TileLoadBalancer *tileLoadBalancer;
  gvt::render::data::accel::AbstractAccel *acceleration;

private:
  std::map<int, gvt::render::actor::RayVector> rayQ;
  std::map<int, gvt::render::actor::RayVector> incomingRayQueue;
  tbb::mutex *rayQMutex;
  tbb::mutex incomingRayQueueMutex;
  pthread_mutex_t rayTransferMutex; // TODO: too coarse grained, use rayQMutex instead
  std::map<gvt::render::data::primitives::Mesh *, gvt::render::Adapter *> adapterCache;

public:
  // image
  tbb::mutex *getColorBufMutex() { return colorBufMutex; }
  gvt::render::data::scene::Image *getImage() { return image; }
  std::vector<glm::vec3> *getFramebuffer() { return &colorBuf; }
  void aggregatePixel(int pixelId, const glm::vec3 &color);
  void updatePixel(int pixelId, const glm::vec3 &color) { colorBuf[pixelId] = color; }
  int decrementPendingPixelCount(int amount) {
    pendingPixelCount -= amount;
    return pendingPixelCount;
  }
  int getImageWidth() const { return imageWidth; }
  int getImageHeight() const { return imageHeight; }

private:
  tbb::mutex *colorBufMutex;
  gvt::render::data::scene::Image *image;
  std::vector<glm::vec3> colorBuf;
  int pendingPixelCount;
  int imageWidth;
  int imageHeight;

  // ray transfer
public:
  bool transferRays();
  void bufferRayTransferWork(RayTransferWork *work);
  void bufferVoteWork(VoteWork *work);
  void voteForResign(int senderRank, unsigned int timeStamp);
  void applyRayTransferResult(int numRays);
  void applyVoteResult(int voteType, unsigned int timeStamp);
  void copyIncomingRays(int instanceId, const gvt::render::actor::RayVector *incomingRays);

  void setProposeAvailable() { voter->setProposeAvailable(); }
  void voteCommit() { voter->voteCommit(); }
  void voteAbort() { voter->voteAbort(); }
  void commit() { voter->commit(); }
  void abort() { voter->abort(); }

private:
  bool hasWork() const;

private:
  int myRank;
  int numRanks;

  void sendRays();
  void receiveRays();

  Voter *voter;

  // Warning: delete these dynamicaly allocated pointers upon copying all rays
  // TODO: minimize resizing
  std::vector<RayTransferWork *> rayTransferBuffer;
  pthread_mutex_t rayTransferBufferLock;

private:
  friend class PixelGatherWork;
  friend class PixelWork;
  friend class RequestWork;

  bool imageReady;
  pthread_mutex_t imageReadyLock;
  pthread_cond_t imageReadyCond;

  bool serverReady;
  pthread_mutex_t serverReadyLock;
  pthread_cond_t serverReadyCond;

private:
  void setupCommon();
  void setupAsyncImage();
  void setupAsyncDomain();
  void setupSyncImage();
  void setupSyncDomain();
  void renderAsyncImage();
  void renderAsyncDomain();
  void renderSyncImage();
  void renderSyncDomain();
  void runDomainTracer();
  void generatePrimaryRays(gvt::render::actor::RayVector &rays);
  void domainTracer(gvt::render::actor::RayVector &rays);
  void filterRaysLocally(gvt::render::actor::RayVector &rays);
  void shuffleRays(gvt::render::actor::RayVector &rays, gvt::core::DBNodeH instNode);
  void shuffleRays(gvt::render::actor::RayVector &rays, int domID);
  void shuffleDropRays(gvt::render::actor::RayVector &rays);
  void clearBuffer() { std::memset(&colorBuf[0], 0, sizeof(glm::vec3) * options.width * options.height); }
  void localComposite();

public:
  // void compositePixels();
  void gatherFramebuffers();
  void gatherTimes();

private:
  gvt::render::Adapter *adapter;
  Profiler profiler;

  bool gatherTimesStart;
  pthread_mutex_t gatherTimesStartMutex;
  pthread_cond_t gatherTimesStartCond;

  bool gatherTimesDone;
  pthread_mutex_t gatherTimesDoneMutex;
  pthread_cond_t gatherTimesDoneCond;
};

class Timer {
public:
  Timer() {
    elapsed = 0.0;
    st = high_resolution_clock::now();
  }
  void start() { st = high_resolution_clock::now(); }
  void stop() {
    auto et = high_resolution_clock::now();
    elapsed = std::chrono::duration<double, std::ratio<1, 1000> >(et - st).count();
  }
  double getElapsed() const { return elapsed; }

private:
  high_resolution_clock::time_point st;
  double elapsed;
};
}
}
}

#endif