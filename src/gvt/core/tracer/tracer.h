/* =======================================================================================
   This file is released as part of GraviT - scalable, platform independent ray tracing
   tacc.github.io/GraviT

   Copyright 2013-2015 Texas Advanced Computing Center, The University of Texas at Austin
   All rights reserved.

   Licensed under the BSD 3-Clause License, (the "License"); you may not use this file
   except in compliance with the License.
   A copy of the License is included with this software in the file LICENSE.
   If your copy does not contain the License, you may obtain a copy of the License at:

       http://opensource.org/licenses/BSD-3-Clause

   Unless required by applicable law or agreed to in writing, software distributed under
   the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.
   See the License for the specific language governing permissions and limitations under
   limitations under the License.

   GraviT is funded in part by the US National Science Foundation under awards ACI-1339863,
   ACI-1339881 and ACI-1339840
   ======================================================================================= */

#ifndef GVT_CORE_TRACER
#define GVT_CORE_TRACER

#include <gvt/core/Debug.h>
#include <gvt/core/comm/comm.h>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace gvt {
namespace core {
    /**
     * @brief Abstract scheduler
     *
     *  Defines the methods required by all derived schedule implementation
     *
     *  Implements the methods required to register and handle messages between nodes
     *
     */
class Scheduler {
protected:
  std::vector<std::string> _registered_messages; /**< Message registery */

public:
    /**
     * @brief Constructor
     */
  Scheduler() {}
  /**
   * @brief Destructor
   */

  virtual ~Scheduler() {}
  /**
   * The scheduling operation
   */
  virtual void operator()() { GVT_ASSERT(false, "Tracer not implemented"); };

  /**
   * Message handling method
   * @param  msg Message received by the communicator @see communicator
   */
  virtual bool MessageManager(std::shared_ptr<gvt::comm::Message> msg) { return false; }

  /**
   * Register Scheduler required messages with the communicator
   * @return [description]
   */
  template <class M> int RegisterMessage() { return gvt::comm::communicator::RegisterMessageType<M>(); }

  /**
   * Check if the node has more work to be done.
   * @return true is work is found, false othewise
   */
  virtual bool isDone() {
    GVT_ASSERT(false, "Tracer not implemented");
    return false;
  }

  /**
   * Check if the node has more work to be done.
   * @return true is work is found, false othewise
   */
  virtual bool hasWork() {
    GVT_ASSERT(false, "Tracer not implemented");
    return false;
  }

  /**
   * Get the cuurent buffer image used the scheduler to accumulate results
   * @return buffer pointer (nullptr if it does not exist)
   */
  virtual float *getImageBuffer() {
    GVT_ASSERT(false, "Tracer not implemented");
    return nullptr;
  };

private:
};
};
};

#endif /*GVT_CORE_TRACER*/
