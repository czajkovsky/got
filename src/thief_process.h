#ifndef THIEF_PROCESS_H_
#define THIEF_PROCESS_H_

#include "sizes.h"
#include "waiting_queue.h"
#include <mpi.h>

class ThiefProcess {
public:
  static ThiefProcess& Get_process() {
    static ThiefProcess process;
    return process;
  }
  void Run(unsigned int rank, Sizes sizes);

  unsigned int Increment_timestamp(unsigned int other_timestamp = 0);

  unsigned int Get_rank() const { return rank_; }
  Sizes Get_sizes() const { return sizes_; }

private:
  ThiefProcess() {}
  ThiefProcess(const ThiefProcess&);
  ThiefProcess& operator=(const ThiefProcess&);

  unsigned int timestamp_;

  unsigned int rank_;
  Sizes sizes_;

  static const unsigned int MESSAGE_LENGTH = 3;
  enum MessageFields {
    RANK_FIELD,
    TIMESTAMP_FIELD,
    QUEUE_FIELD
  };

  static const unsigned int PARTNERSHIP_Q_ID = 0;
  static const unsigned int DOCUMENTATION_Q_ID = 1;

  int requests_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  int releases_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  int confirms_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  MPI::Request request_requests_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request release_requests_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request confirm_requests_[Sizes::MAX_NUMBER_OF_THIEVES];

  WaitingQueue partnership_queue_;
  WaitingQueue documentation_queue_;
  WaitingQueue houses_queue_[Sizes::MAX_NUMBER_OF_HOUSES];

  unsigned int entry_timestamp_;

  enum CommunicationTAG {
    REQUEST_TAG,
    CONFIRM_TAG,
    RELEASE_TAG
  };

  void (ThiefProcess::*state_)();

  // TODO change names and behaviour below
  void Partnership_insert();
  void Partnership_wait_for_confirm();
  void Partnership_wait_for_top();
  void Partnership_critical_section();
  void Partnership_release();
  void Partnership_wait_for_partner();
  void Partnership_notify_partner();

  void Set_up_communication();
  void Main_loop();
  void Try_communication();
  void Try_release_resources();
};

#endif  // THIEF_PROCESS_H_
