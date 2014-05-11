#ifndef THIEF_PROCESS_H_
#define THIEF_PROCESS_H_

#include "sizes.h"
#include "waiting_process_priority_queue.h"
#include "left_house_queue.h"
#include "message.h"
#include "communicator.h"
#include "time_point.h"

class ThiefProcess {
public:
  ThiefProcess(Sizes sizes);

  void Run();

  int Increment_timestamp(int other_timestamp = 0);

  int Get_rank() const { return communicator_.Get_rank(); }
  Sizes Get_sizes() const { return sizes_; }

private:
  ThiefProcess(const ThiefProcess&);
  ThiefProcess& operator=(const ThiefProcess&);

  int timestamp_;
  TimePoint sleep_start_;

  Sizes sizes_;

  enum QueueID {
    PARTNERSHIP_Q_ID,
    HOUSE_Q_ID
  };

  static const int PAPERWORK_DURATION = 3;
  static const int BURGLARY_DURATION = 7;
  static const int HOUSE_QUARANTINE_DURATION = 3;

  Communicator communicator_;

  Message request_[Sizes::MAX_NUMBER_OF_THIEVES];
  Message release_[Sizes::MAX_NUMBER_OF_THIEVES];
  Message confirm_[Sizes::MAX_NUMBER_OF_THIEVES];
  Message partner_sync_;

  WaitingProcessPriorityQueue partnership_queue_;
  WaitingProcessPriorityQueue waiting_houses_queue_[Sizes::MAX_NUMBER_OF_HOUSES];
  LeftHouseQueue left_houses_queue_;

  int entry_timestamp_;
  int house_entry_timestamp_[Sizes::MAX_NUMBER_OF_HOUSES];

  void (ThiefProcess::*state_)();

  // TODO change names and behaviour below
  void Partnership_insert();
  void Partnership_wait_for_confirm();
  void Partnership_wait_for_top();
  void Partnership_critical_section();
  void Partnership_release();
  void Partnership_wait_for_partner();
  void Partnership_notify_partner();

  void Docs_wait_for_top();
  void Docs_critical_section();
  void Docs_wait_for_partner();
  void Docs_start_waiting_for_partner();

  void House_request_entry();
  void House_wait_for_confirm();
  void House_critical_section();
  void House_start_waiting_for_partner();
  void House_wait_for_partner();
  void House_notify_partner();

  int current_partner_rank_;
  int current_house_id_;

  void Set_up_communication();
  void Main_loop();
  void Try_communication();
  void Try_release_resources();
};

#endif  // THIEF_PROCESS_H_
