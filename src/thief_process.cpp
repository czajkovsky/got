#include "thief_process.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>
#include <time.h>

#include "sizes.h"

void ThiefProcess::Run(unsigned int rank, Sizes sizes) {
  rank_ = rank;
  sizes_ = sizes;
  sleep_start_ = time_t(-1);

  state_ = &ThiefProcess::Partnership_insert;

  std::cout << "Starting ThiefProcess [rank = " << Get_rank() << "] "
    << "[thieves = " << Get_sizes().Get_number_of_thieves()
    << ", desks = " << Get_sizes().Get_number_of_desks()
    << ", houses = " << Get_sizes().Get_number_of_houses()
    << "]" << std::endl;

  Set_up_communication();
  Main_loop();
}

void ThiefProcess::Set_up_communication() {
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    request_requests_[i] = MPI::COMM_WORLD.Irecv(requests_[i], MESSAGE_LENGTH, MPI_INT, i, REQUEST_TAG);
    release_requests_[i] = MPI::COMM_WORLD.Irecv(releases_[i], MESSAGE_LENGTH, MPI_INT, i, RELEASE_TAG);
  }
}

void ThiefProcess::Main_loop() {
  while (true)
  {
    Try_communication();
    (this->*state_)();
    Try_release_resources();

    // usleep(100000);  // TODO remove it later
  }
}

void ThiefProcess::Try_communication() {
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++)  {
    if (i == Get_rank()) continue;
    if (request_requests_[i].Test()) {
      unsigned int current_timestamp = Increment_timestamp(requests_[i][TIMESTAMP_FIELD]);

      if (requests_[i][QUEUE_FIELD] == PARTNERSHIP_Q_ID) {
        partnership_queue_.Insert(WaitingProcess(requests_[i][TIMESTAMP_FIELD], requests_[i][RANK_FIELD]));
        // std::cout << "[" << Get_rank() << "] " "request from " << requests_[i][RANK_FIELD] << std::endl;
      } else if (requests_[i][QUEUE_FIELD] == DOCUMENTATION_Q_ID) {
        documentation_queue_.Insert(WaitingProcess(requests_[i][TIMESTAMP_FIELD], requests_[i][RANK_FIELD]));
      }
      else {
        int house_id = requests_[i][QUEUE_FIELD] - PARTNERSHIP_Q_ID;
        houses_queue_[house_id].Insert(WaitingProcess(requests_[i][TIMESTAMP_FIELD], requests_[i][RANK_FIELD]));
      }

      requests_[i][TIMESTAMP_FIELD] = current_timestamp;
      MPI::COMM_WORLD.Send(requests_[i], MESSAGE_LENGTH, MPI_INT, requests_[i][RANK_FIELD], CONFIRM_TAG);

      request_requests_[i] = MPI::COMM_WORLD.Irecv(requests_[i], MESSAGE_LENGTH, MPI_INT, i, REQUEST_TAG);
    }
  }
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    if (release_requests_[i].Test()) {
      Increment_timestamp(releases_[i][TIMESTAMP_FIELD]);

      if (releases_[i][QUEUE_FIELD] == PARTNERSHIP_Q_ID) {
        partnership_queue_.Erase(
          partnership_queue_.After(WaitingProcess(releases_[i][ENTRY_FIELD], releases_[i][RANK_FIELD]))
        );
        partnership_queue_.Erase(
          WaitingProcess(releases_[i][ENTRY_FIELD], releases_[i][RANK_FIELD])
        );
      } else if (releases_[i][QUEUE_FIELD] == DOCUMENTATION_Q_ID) {
        documentation_queue_.Erase(
          WaitingProcess(releases_[i][ENTRY_FIELD], releases_[i][RANK_FIELD])
        );
      }
      else {
        int house_id = releases_[i][QUEUE_FIELD] - PARTNERSHIP_Q_ID;
        houses_queue_[house_id].Pop();
      }

      release_requests_[i] = MPI::COMM_WORLD.Irecv(releases_[i], MESSAGE_LENGTH, MPI_INT, i, RELEASE_TAG);
    }
  }
}

void ThiefProcess::Try_release_resources() {
  // TODO release houses etc...
}

void ThiefProcess::Partnership_insert() {
  entry_timestamp_ = Increment_timestamp();

  int msg[MESSAGE_LENGTH];
  msg[RANK_FIELD]       = static_cast<int>(Get_rank());
  msg[TIMESTAMP_FIELD]  = static_cast<int>(entry_timestamp_);
  msg[QUEUE_FIELD]      = PARTNERSHIP_Q_ID;

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, i, REQUEST_TAG);
  }

  partnership_queue_.Insert(WaitingProcess(entry_timestamp_, Get_rank()));

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    confirm_requests_[i] = MPI::COMM_WORLD.Irecv(confirms_[i], MESSAGE_LENGTH, MPI_INT, i, CONFIRM_TAG);
  }
//if (Get_rank() == 0) {
//  std::cout << "[" << Get_rank() << "] " "printing" << std::endl;
//  partnership_queue_.Print();
//}

  state_ = &ThiefProcess::Partnership_wait_for_confirm;
}

void ThiefProcess::Partnership_wait_for_confirm() {
  bool confirmed = true;
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    if (!confirm_requests_[i].Test()) {
      confirmed = false;
    }
  }
  if (confirmed) {
    //std::cout << "[" << Get_rank() << "] " "confirmed" << std::endl;
    state_ = &ThiefProcess::Partnership_wait_for_top;
  }
}

void ThiefProcess::Partnership_wait_for_top() {
//if (Get_rank() == 0) {
//  std::cout << "[" << Get_rank() << "] " "printing" << std::endl;
//  partnership_queue_.Print();
//}
  if (partnership_queue_.Is_in_top(Get_sizes().Get_number_of_thieves(), WaitingProcess(entry_timestamp_, Get_rank()))) {
    state_ = &ThiefProcess::Partnership_critical_section;
  }
}

void ThiefProcess::Partnership_critical_section() {
  std::cout << "[" << Get_rank() << "] " "partnership critical section" << std::endl;
  //partnership_queue_.Print();

  if (partnership_queue_.Position_of(WaitingProcess(entry_timestamp_, Get_rank())) % 2 == 1) {
    Increment_timestamp();
    partner_request_ = MPI::COMM_WORLD.Irecv(partner_, MESSAGE_LENGTH, MPI_INT, MPI_ANY_SOURCE, PARTNER_TAG);
    state_ = &ThiefProcess::Partnership_wait_for_partner;
  } else {
    current_partner_rank_ = partnership_queue_.Before(WaitingProcess(entry_timestamp_, Get_rank())).Get_rank();
    state_ = &ThiefProcess::Partnership_notify_partner;
  }
}

void ThiefProcess::Partnership_wait_for_partner() {
  if (partner_request_.Test()) {
    Increment_timestamp(partner_[TIMESTAMP_FIELD]);
    current_partner_rank_ = partner_[RANK_FIELD];
    std::cout << "[a] rank = " << Get_rank() << ", partner = " << current_partner_rank_ << std::endl;
    state_ = &ThiefProcess::Partnership_release;
  }
}

void ThiefProcess::Partnership_notify_partner() {
  unsigned int current_timestamp = Increment_timestamp();
  int msg[MESSAGE_LENGTH];
  msg[RANK_FIELD]       = static_cast<int>(Get_rank());
  msg[TIMESTAMP_FIELD]  = static_cast<int>(current_timestamp);

  MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, current_partner_rank_, PARTNER_TAG);

  std::cout << "[b] rank = " << Get_rank() << ", partner = " << current_partner_rank_ << std::endl;
  state_ = &ThiefProcess::Docs_start_waiting_for_partner;
  // state_ = &ThiefProcess::Partnership_insert;
}

void ThiefProcess::Partnership_release() {
  assert(static_cast<int>(Get_rank()) != current_partner_rank_);
  assert(current_partner_rank_ > -1);

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    unsigned int current_timestamp = Increment_timestamp();

    int msg[MESSAGE_LENGTH];
    msg[RANK_FIELD]       = static_cast<int>(Get_rank());
    msg[TIMESTAMP_FIELD]  = static_cast<int>(current_timestamp);
    msg[QUEUE_FIELD]      = PARTNERSHIP_Q_ID;
    msg[ENTRY_FIELD]      = entry_timestamp_;

    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, i, RELEASE_TAG);
  }

  partnership_queue_.Erase(
    partnership_queue_.After(WaitingProcess(entry_timestamp_, Get_rank()))
  );
  partnership_queue_.Erase(
    WaitingProcess(entry_timestamp_, Get_rank())
  );

  state_ = &ThiefProcess::Docs_request_entry;
  // state_ = &ThiefProcess::Partnership_insert;
}

void ThiefProcess::Docs_request_entry() {
  entry_timestamp_ = Increment_timestamp();

  int msg[MESSAGE_LENGTH];
  msg[RANK_FIELD]       = static_cast<int>(Get_rank());
  msg[TIMESTAMP_FIELD]  = static_cast<int>(entry_timestamp_);
  msg[QUEUE_FIELD]      = DOCUMENTATION_Q_ID;

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, i, REQUEST_TAG);
  }

  documentation_queue_.Insert(WaitingProcess(entry_timestamp_, Get_rank()));

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    confirm_requests_[i] = MPI::COMM_WORLD.Irecv(confirms_[i], MESSAGE_LENGTH, MPI_INT, i, CONFIRM_TAG);
  }

  state_ = &ThiefProcess::Docs_wait_for_confirm;
}

void ThiefProcess::Docs_wait_for_confirm() {
  bool confirmed = true;
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    if (!confirm_requests_[i].Test()) {
      confirmed = false;
    }
  }
  if (confirmed) {
    state_ = &ThiefProcess::Docs_wait_for_top;
  }
}

void ThiefProcess::Docs_wait_for_top() {
  int max_in_room = Get_sizes().Get_number_of_desks() / 2;
  if (documentation_queue_.Is_in_top(max_in_room, WaitingProcess(entry_timestamp_, Get_rank()))) {
    state_ = &ThiefProcess::Docs_critical_section;
  }
}

void ThiefProcess::Docs_critical_section() {
  if (sleep_start_ == time_t(-1)) {
    time(&sleep_start_);
    std::cout << "[" << Get_rank() << "] " "has started filling the docs (critical section)" << std::endl;
  } else {
    time_t now;
    time(&now);
    if (difftime(now, sleep_start_) > PAPERWORK_DURATION) {
      std::cout << "[" << Get_rank() << "] " "has finished filling the docs" << std::endl;
      sleep_start_ = time_t(-1);
      state_ = &ThiefProcess::Docs_release;
    }
  }
}

void ThiefProcess::Docs_release() {

  int msg[MESSAGE_LENGTH];
  MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, current_partner_rank_, DOCS_TAG);
  state_ = &ThiefProcess::Partnership_insert;

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    Increment_timestamp();

    int msg[MESSAGE_LENGTH];
    msg[RANK_FIELD]       = static_cast<int>(Get_rank());
    msg[TIMESTAMP_FIELD]  = static_cast<int>(entry_timestamp_);
    msg[QUEUE_FIELD]      = DOCUMENTATION_Q_ID;
    msg[ENTRY_FIELD]      = entry_timestamp_;

    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, i, RELEASE_TAG);
  }

  documentation_queue_.Erase(WaitingProcess(entry_timestamp_, Get_rank()));
}

void ThiefProcess::Docs_start_waiting_for_partner() {
  std::cout << "[" << Get_rank()<< "] has started waiting for the docs" << std::endl;
  int msg[MESSAGE_LENGTH];
  docs_request_ = MPI::COMM_WORLD.Irecv(msg, MESSAGE_LENGTH, MPI_INT, MPI_ANY_SOURCE, DOCS_TAG);
  state_ = &ThiefProcess::Docs_wait_for_partner;
}

void ThiefProcess::Docs_wait_for_partner() {
  if (docs_request_.Test()) {
    std::cout << "[" << Get_rank()<< "] "
    << current_partner_rank_ << " - has filled the docs" << std::endl;
    state_ = &ThiefProcess::Partnership_insert;
  }
}

unsigned int ThiefProcess::Increment_timestamp(unsigned int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}
