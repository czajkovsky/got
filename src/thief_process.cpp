#include "thief_process.h"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <mpi.h>
#include <unistd.h>

#include "sizes.h"

void ThiefProcess::Run(unsigned int rank, Sizes sizes) {
  rank_ = rank;
  sizes_ = sizes;

  state_ = &ThiefProcess::Partnership_insert;

  waiting_for_partner_rank_ = -1;

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

    usleep(100000);  // TODO remove it later
  }
}

void ThiefProcess::Try_communication() {
  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++)  {
    if (i == Get_rank()) continue;
    if (request_requests_[i].Test()) {
      unsigned int current_timestamp = Increment_timestamp();

      // TODO remove magic numbers

      if (requests_[i][QUEUE_FIELD] == PARTNERSHIP_Q_ID) {
        partnership_queue_.Insert(WaitingProcess(requests_[i][TIMESTAMP_FIELD], requests_[i][RANK_FIELD]));
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
      Increment_timestamp();

      if (releases_[i][QUEUE_FIELD] == PARTNERSHIP_Q_ID) {
        partnership_queue_.Pop();
        if (waiting_for_partner_rank_ == -1) {
          waiting_for_partner_rank_ = requests_[i][0];
        } else {
          waiting_for_partner_rank_ = -1;
        }
      } else if (requests_[i][QUEUE_FIELD] == DOCUMENTATION_Q_ID) {
        documentation_queue_.Pop();
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
    state_ = &ThiefProcess::Partnership_wait_for_top;
  }
}

void ThiefProcess::Partnership_wait_for_top() {
  if (partnership_queue_.Is_on_top(WaitingProcess(entry_timestamp_, Get_rank()))) {
    state_ = &ThiefProcess::Partnership_critical_section;
  }
}

void ThiefProcess::Partnership_critical_section() {
  std::cout << "[" << Get_rank() << "] " "partnership critical section" << std::endl;

  state_ = &ThiefProcess::Partnership_release;
}

void ThiefProcess::Partnership_release() {
  if (waiting_for_partner_rank_ == -1) {
    waiting_for_partner_rank_ = Get_rank();
    partner_request_ = MPI::COMM_WORLD.Irecv(&partner_, MESSAGE_LENGTH, MPI_INT, MPI_ANY_SOURCE, PARTNER_TAG);
    state_ = &ThiefProcess::Partnership_wait_for_partner;
  } else {
    int msg[MESSAGE_LENGTH];
    msg[RANK_FIELD]       = static_cast<int>(Get_rank());
    msg[TIMESTAMP_FIELD]  = static_cast<int>(entry_timestamp_);
    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, waiting_for_partner_rank_, PARTNER_TAG);
    current_partner_rank_ = waiting_for_partner_rank_;
    waiting_for_partner_rank_ = -1;

    std::cout << "[b] rank = " << Get_rank() <<", partner = " << current_partner_rank_ << std::endl;
    state_ = &ThiefProcess::Partnership_insert;
  }

  for (unsigned int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    Increment_timestamp();

    int msg[MESSAGE_LENGTH];
    msg[RANK_FIELD]       = static_cast<int>(Get_rank());
    msg[TIMESTAMP_FIELD]  = static_cast<int>(entry_timestamp_);
    msg[QUEUE_FIELD]      = PARTNERSHIP_Q_ID;

    MPI::COMM_WORLD.Send(msg, MESSAGE_LENGTH, MPI_INT, i, RELEASE_TAG);
  }

  partnership_queue_.Pop();
}

void ThiefProcess::Partnership_wait_for_partner() {
  if (partner_request_.Test()) {
    current_partner_rank_ = partner_[RANK_FIELD];
    std::cout << "[a] rank = " << Get_rank() << ", partner = " << current_partner_rank_ << std::endl;
    state_ = &ThiefProcess::Partnership_insert;
  }
}

unsigned int ThiefProcess::Increment_timestamp(unsigned int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}
