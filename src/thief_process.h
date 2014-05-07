#ifndef THIEF_PROCESS_H_
#define THIEF_PROCESS_H_

#include "sizes.h"
#include "waiting_process_priority_queue.h"
#include "left_house_queue.h"
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
  time_t sleep_start_;
  time_t house_sleep_start_;

  unsigned int rank_;
  Sizes sizes_;

  static const unsigned int MESSAGE_LENGTH = 4;
  enum MessageFields {
    RANK_FIELD,
    TIMESTAMP_FIELD,
    QUEUE_FIELD,
    ENTRY_FIELD
  };

  enum QueueID {
    PARTNERSHIP_Q_ID,
    DOCUMENTATION_Q_ID,
    HOUSE_Q_ID
  };

  static const unsigned int PAPERWORK_DURATION = 3;
  static const unsigned int BURGLARY_DURATION = 2;

  int requests_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  int releases_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  int confirms_[Sizes::MAX_NUMBER_OF_THIEVES][MESSAGE_LENGTH];
  int partner_[MESSAGE_LENGTH];
  int docs_[MESSAGE_LENGTH];
  MPI::Request request_requests_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request release_requests_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request confirm_requests_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request partner_request_;
  MPI::Request docs_request_;

  WaitingProcessPriorityQueue partnership_queue_;
  WaitingProcessPriorityQueue documentation_queue_;
  WaitingProcessPriorityQueue waiting_houses_queue_[Sizes::MAX_NUMBER_OF_HOUSES];
  LeftHouseQueue left_houses_queue_;

  unsigned int entry_timestamp_;
  int house_entry_timestamp_[Sizes::MAX_NUMBER_OF_HOUSES];

  enum CommunicationTAG {
    REQUEST_TAG,
    CONFIRM_TAG,
    RELEASE_TAG,
    PARTNER_TAG,
    DOCS_TAG
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

  void Docs_request_entry();
  void Docs_wait_for_confirm();
  void Docs_wait_for_top();
  void Docs_critical_section();
  void Docs_release();

  void Docs_wait_for_partner();
  void Docs_start_waiting_for_partner();

  void House_request_entry();
  void House_wait_for_confirm();
  void House_critical_section();

  int current_partner_rank_;

  void Broadcast(int msg[], int tag_type);

  void Set_up_communication();
  void Main_loop();
  void Try_communication();
  void Try_release_resources();
};

#endif  // THIEF_PROCESS_H_
