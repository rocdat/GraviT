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
// RayTallyWork.cpp
//

#include "gvt/render/unit/RayTallyWork.h"
#include "gvt/render/unit/MpiRenderer.h"
#include "gvt/core/mpi/Work.h"
#include "gvt/render/actor/Ray.h"

using namespace std;
using namespace gvt::core::mpi;
using namespace gvt::render::unit;
using namespace gvt::render::actor;

// #define DEBUG_RAY_WORK
#define FIND_THE_BUG

WORK_CLASS(RayTallyWork)

void RayTallyWork::Serialize(size_t &size, unsigned char *&serialized) {
  size = 0;
  serialized = NULL;
}

Work *RayTallyWork::Deserialize(size_t size, unsigned char *serialized) {
  if (size != 0) {
    std::cerr << "DoneTestWork deserializer call with size != 0 rank " << Application::GetApplication()->GetRank()
              << "\n";
    exit(1);
  }
  RayTallyWork *work = new RayTallyWork;
  return work;
}

bool RayTallyWork::Action() {

  MpiRenderer *renderer = static_cast<MpiRenderer *>(Application::GetApplication());
  std::map<int, RayVector> *rayQ = renderer->getRayQueue();
  int numRanks = renderer->GetSize();
  int myRank = renderer->GetRank();

  printf("RayTallyWork::Action: Rank %d: waiting for all localRayCountDone flags to set\n", myRank);

  int readyForTest;
  do {
    readyForTest = 0;
    int done = renderer->isLocalRayCountDone();
    MPI_Allreduce(&done, &readyForTest, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  } while(readyForTest != numRanks);

  printf("RayTallyWork::Action: Rank %d: all localRayCountDone flags set to true\n", myRank);

  std::vector<unsigned int>* rayCounts = renderer->getRayCounts();
  std::vector<unsigned int> numRaysToReceive = std::vector<unsigned int>(numRanks, 0);

  MPI_Allreduce(&(*rayCounts)[0], &numRaysToReceive[0], numRanks, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);

#ifdef FIND_THE_BUG
  printf("RayTallyWork::Action: Rank %d: setting num rays to receive to %d\n", myRank, numRaysToReceive[myRank]);
#endif

  renderer->setNumRaysToReceive(numRaysToReceive[myRank]);

  if (numRaysToReceive[myRank] == 0) {
    printf("Rank %d: RayTallyWork: setting ray transfer done to true!!!\n", myRank);
    renderer->setRayTransferDone(true);
  }

  renderer->setRayTallyDone(true);
  
  return false;
}
