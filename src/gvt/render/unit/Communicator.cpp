#include "gvt/render/unit/Communicator.h"

// works
#include "gvt/render/unit/DomainWorks.h"
#include "gvt/render/unit/Work.h"

// tracers
#include "gvt/render/unit/RayTracer.h"

// TPC voter
#include "gvt/render/unit/TpcVoter.h"

#include <mpi.h>
#include <iostream>

namespace gvt {
namespace render {
namespace unit {

// Communicator::Communicator() { Initialize(); }

// Communicator::Communicator(int* argc, char*** argv, Worker* worker)
//     : argcp(argc), argvp(argv), worker(worker), allWorkDone(false) {
Communicator::Communicator(const MpiInfo& mpi, Worker* worker)
    : mpi(mpi), worker(worker), allWorkDone(false) {
  InitThreads();
}

// void Communicator::Initialize() {
//   allWorkDone = false;
//   voter = tracer->GetVoter();
//
//   // initialization
//   InitMpi();
//   InitThreads();
//
//   // // create a voter
//   // if (mpi.size > 1) voter = new TpcVoter(mpi.size, mpi.rank, *tracer,
//   this);
// }

int Communicator::RegisterWork(Work* (*Deserialize)()) {
  int tag = deserializers.size();
  deserializers.push_back(Deserialize);
  return tag;
}

void Communicator::Send(Work* work) {
  pthread_mutex_lock(&sendQ_mutex);
  sendQ.push(work);
#ifndef NDEBUG
  std::cout << "rank " << mpi.rank << " pushed work tag " << work->GetTag()
            << " to sendQ (size " << sendQ.size() << ")" << std::endl;
#endif
  pthread_mutex_unlock(&sendQ_mutex);
}

// void Communicator::InitMpi() {
//   // warning: this should be in the beginning of main()
//   // int provided;
//   // MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
//   // if (provided != MPI_THREAD_SINGLE) {
//   //   std::cout << "error mpi_thread_single not available\n";
//   //   exit(1);
//   // }
//
//   // warning: MPI_Comm_dup is causing some problem.
//   // directly use MPI_COMM_WORLD for now.
//   // int dup_error = MPI_Comm_dup(MPI_COMM_WORLD, &mpi.comm);
//   // if (dup_error != MPI_SUCCESS) {
//   //   std::cout << "error MPI_Comm_dup not successful\n";
//   //   exit(1);
//   // }
//   mpi.comm = MPI_COMM_WORLD;
//   MPI_Comm_rank(mpi.comm, &mpi.rank);
//   MPI_Comm_size(mpi.comm, &mpi.size);
// }

void Communicator::InitThreads() {
  InitMutexes();

  threads.resize(NUM_PTHREADS);

  int error = pthread_create(&threads[WORK_THREAD], NULL,
                         &Communicator::StartWorkThread, this);
  if (error) {
    std::cout << "error " << error << " failed to create worker thread.\n";
    exit(error);
  }

  error = pthread_create(&threads[MESSAGE_THREAD], NULL,
                         &Communicator::StartMessageThread, this);
  if (error) {
    std::cout << "error " << error << " failed to create message thread.\n";
    exit(error);
  }
}

void Communicator::InitMutexes() {
  pthread_mutex_init(&sendQ_mutex, NULL);
  pthread_mutex_init(&recvQ_mutex, NULL);
}

void* Communicator::StartWorkThread(void* This) {
  Communicator* worker = static_cast<Communicator*>(This);
  worker->WorkThread();
  return NULL;
}

void* Communicator::StartMessageThread(void* This) {
  Communicator* worker = static_cast<Communicator*>(This);
  worker->MessageThread();
  return NULL;
}

inline void Communicator::WorkThread() {
  while (!allWorkDone) {
    Work* work = NULL;

    pthread_mutex_lock(&recvQ_mutex);
    if (!recvQ.empty()) {
      work = recvQ.front();
      recvQ.pop();
#ifndef NDEBUG
      std::cout << "rank " << mpi.rank << " popped work tag " << work->GetTag()
                << " from recvQ (size " << recvQ.size() << ")" << std::endl;
#endif
    }
    pthread_mutex_unlock(&recvQ_mutex);

    if (work) {
      bool delete_this = work->Action(worker);
      if (delete_this) delete work;
    }
  }
}

void Communicator::MessageThread() {

  bool done = false;
  MPI_Status mpi_status;

  // int pvd;
  // MPI_Init_thread(argcp, argvp, MPI_THREAD_MULTIPLE, &pvd);
  // if ((pvd != MPI_THREAD_MULTIPLE)) {
  //   std::cerr << "error: mpi_thread_multiple not available\n";
  //   exit(1);
  // }
  // // MPI_Init(argcp, argvp);

  // MPI_Comm_rank(MPI_COMM_WORLD, &mpi.rank);
  // MPI_Comm_size(MPI_COMM_WORLD, &mpi.size);
  
  // worker->SignalMpiReady();
  worker->WaitTracerReady();

  while (!allWorkDone) {
    // serve incoming message
    int flag;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &mpi_status);

    if (flag) {
      assert(mpi_status.MPI_TAG >= 0 &&
             mpi_status.MPI_TAG < deserializers.size());
      Work* incoming_work = deserializers[mpi_status.MPI_TAG]();
      RecvWork(mpi_status, incoming_work);
    }

    // serve outgoing message
    Work* outgoing_work = NULL;
    pthread_mutex_lock(&sendQ_mutex);
    if (!sendQ.empty()) {
      outgoing_work = sendQ.front();
      sendQ.pop();
#ifndef NDEBUG
      std::cout << "rank " << mpi.rank << " popped work tag "
                << outgoing_work->GetTag() << " from sendQ (size "
                << sendQ.size() << ")" << std::endl;
#endif
    }
    pthread_mutex_unlock(&sendQ_mutex);

    if (outgoing_work) SendWork(outgoing_work);
  }
}

void Communicator::Quit() {
#ifndef NDEBUG
  std::cout << "rank " << mpi.rank << " seting allWorkDOne = 1 "
            << __PRETTY_FUNCTION__ << std::endl;
#endif
  allWorkDone = true;
  for (std::size_t i = 0; i < threads.size(); ++i) {
    pthread_join(threads[i], NULL);
#ifndef NDEBUG
    std::cout << "rank " << mpi.rank << " thread " << i << " / "
              << threads.size() << " joined." << std::endl;
#endif
  }
  // error checking code
  // we don't need lock/unlock but just in case
  pthread_mutex_lock(&sendQ_mutex);
  if (!sendQ.empty()) {
    std::cout << "error rank " << mpi.rank << " send queue not empty. size "
              << sendQ.size() << "\n";
    exit(1);
  }
  pthread_mutex_unlock(&sendQ_mutex);

  pthread_mutex_lock(&recvQ_mutex);
  if (!recvQ.empty()) {
    std::cout << "error rank " << mpi.rank << " recv queue not empty. size "
              << recvQ.size() << "\n";
    exit(1);
  }
  pthread_mutex_unlock(&recvQ_mutex);
}

void Communicator::RecvWork(const MPI_Status& status, Work* work) {
  int count = 0;
  MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);

#ifndef NDEBUG
  std::cout << "rank " << mpi.rank << " count " << count << " "
            << __PRETTY_FUNCTION__ << "\n";
#endif

#ifdef DONT_ALLOW_ZERO_BYTE
  if (count < 1) {
    std::cout << "error unable to receive " << count << " bytes. tag "
              << status.MPI_TAG << " source " << status.MPI_SOURCE << "\n";
    exit(1);
  }
#endif

  work->Allocate(count);
#ifndef NDEBUG
  printf("[MPI_Recv] buf %p count %d src %d tag %d\n", work->GetBuffer(), count,
         status.MPI_SOURCE, status.MPI_TAG);
#endif
  MPI_Status status_out;
  MPI_Recv(work->GetBuffer(), count, MPI_UNSIGNED_CHAR, status.MPI_SOURCE,
           status.MPI_TAG, MPI_COMM_WORLD, &status_out);

  int count_recved = 0;
  MPI_Get_count(&status_out, MPI_UNSIGNED_CHAR, &count_recved);
  if (count_recved != count) {
    std::cout << "error count mismatch!\n";
    exit(1);
  }

  pthread_mutex_lock(&recvQ_mutex);
  recvQ.push(work);
#ifndef NDEBUG
  std::cout << "rank " << mpi.rank << " pushed work tag " << work->GetTag()
            << " to recvQ (size " << recvQ.size() << ")" << std::endl;
#endif
  pthread_mutex_unlock(&recvQ_mutex);
}

void Communicator::SendWork(Work* work) {
  int comm_type = work->GetCommType();
  if (comm_type == Work::SEND_ALL_OTHER) {
    SendWorkAllOther(work);
    delete work;
  } else if (comm_type == Work::SEND_ALL) {
    SendWorkAllOther(work);

    pthread_mutex_lock(&recvQ_mutex);
    recvQ.push(work);
    pthread_mutex_unlock(&recvQ_mutex);
  } else {  // P2P
    // TODO
    int count = work->GetSize();
#ifdef DONT_ALLOW_ZERO_BYTE
    if (count < 1) {
      std::cout << "error unable to send " << count << " bytes.\n";
      exit(1);
    }
#endif
    if (work->GetBuffer() == NULL) {
      std::cout << "NULL detected\n";
      exit(1);
    } 
#ifndef NDEBUG
    printf("[MPI_Send] buf %p count %d dest %d tag %d\n", work->GetBuffer(), count,
           work->GetDestination(), work->GetTag());
#endif
    MPI_Send(work->GetBuffer(), count, MPI_UNSIGNED_CHAR,
             work->GetDestination(), work->GetTag(), MPI_COMM_WORLD);
    delete work;
  }
}

void Communicator::SendWorkAllOther(Work* work) {
  for (int dest = 0; dest < mpi.size; ++dest) {
    if (dest != mpi.rank) {
      MPI_Send(work->GetBuffer(), work->GetSize(), MPI_UNSIGNED_CHAR, dest,
               work->GetTag(), MPI_COMM_WORLD);
    }
  }
}

void Communicator::IsendWork(Work* work) {
  // TBD
}

}  // namespace unit
}  // namespace render
}  // namespace gvt
