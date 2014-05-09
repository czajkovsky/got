#include "thief_process.h"

#include <algorithm>
#include <cstdlib>
#include <mpi.h>

#include "message.h"
#include "communicator.h"
#include "logger.h"
#include "sizes.h"
#include "time_point.h"

ThiefProcess::ThiefProcess(Sizes sizes)
  : sleep_start_(TimePoint::Uninitialized()),
    sizes_(sizes),
    communicator_() {
}

void ThiefProcess::Run() {
  sleep_start_ = TimePoint::Uninitialized();

  std::fill_n(house_entry_timestamp_, Sizes::MAX_NUMBER_OF_HOUSES, -1);

  state_ = &ThiefProcess::Partnership_insert;

  LOG_INFO("Starting ThiefProcess"
    << " [thieves = " << Get_sizes().Get_number_of_thieves()
    << ", desks = " << Get_sizes().Get_number_of_desks()
    << ", houses = " << Get_sizes().Get_number_of_houses()
    << "]")

  Set_up_communication();
  Main_loop();
}

void ThiefProcess::Set_up_communication() {
  communicator_.Irecv_all(request_, Communicator::REQUEST_TAG);
  communicator_.Irecv_all(release_, Communicator::RELEASE_TAG);
}

void ThiefProcess::Main_loop() {
  while (true) {
    Try_communication();
    (this->*state_)();
    Try_release_resources();
  }
}

void ThiefProcess::Try_communication() {
  for (int i=0; i<Get_sizes().Get_number_of_thieves(); i++)  {
    if (i == Get_rank()) continue;
    if (communicator_.Test(i, Communicator::REQUEST_TAG)) {
      int current_timestamp
        = Increment_timestamp(request_[i].Get(Message::TIMESTAMP_FIELD));

      if (request_[i].Get(Message::QUEUE_FIELD) == PARTNERSHIP_Q_ID) {
        partnership_queue_.Insert(WaitingProcess(request_[i].Get(Message::TIMESTAMP_FIELD), request_[i].Get(Message::RANK_FIELD)));
      } else if (request_[i].Get(Message::QUEUE_FIELD) == DOCUMENTATION_Q_ID) {
        documentation_queue_.Insert(WaitingProcess(request_[i].Get(Message::TIMESTAMP_FIELD), request_[i].Get(Message::RANK_FIELD)));
      } else {
        const int house_id = request_[i].Get(Message::QUEUE_FIELD) - HOUSE_Q_ID;
        if (house_entry_timestamp_[house_id] == -1 ||
          WaitingProcess(request_[i].Get(Message::TIMESTAMP_FIELD), request_[i].Get(Message::RANK_FIELD)) < WaitingProcess(house_entry_timestamp_[house_id], Get_rank())) {
        } else {
          waiting_houses_queue_[house_id].Insert(WaitingProcess(request_[i].Get(Message::TIMESTAMP_FIELD), request_[i].Get(Message::RANK_FIELD)));
          communicator_.Irecv(i, &request_[i], Communicator::REQUEST_TAG);
          continue;
        }
      }

      request_[i].Set(Message::TIMESTAMP_FIELD, current_timestamp);
      communicator_.Send(request_[i].Get(Message::RANK_FIELD),
        request_[i], Communicator::CONFIRM_TAG);

      communicator_.Irecv(i, &request_[i], Communicator::REQUEST_TAG);
    }
  }
  for (int i=0; i<Get_sizes().Get_number_of_thieves(); i++) {
    if (i == Get_rank()) continue;
    if (communicator_.Test(i, Communicator::RELEASE_TAG)) {
      Increment_timestamp(release_[i].Get(Message::TIMESTAMP_FIELD));

      if (release_[i].Get(Message::QUEUE_FIELD) == PARTNERSHIP_Q_ID) {
        partnership_queue_.Erase(
          partnership_queue_.After(WaitingProcess(release_[i].Get(Message::ENTRY_FIELD), release_[i].Get(Message::RANK_FIELD)))
        );
        partnership_queue_.Erase(
          WaitingProcess(release_[i].Get(Message::ENTRY_FIELD), release_[i].Get(Message::RANK_FIELD))
        );
      } else if (release_[i].Get(Message::QUEUE_FIELD) == DOCUMENTATION_Q_ID) {
        documentation_queue_.Erase(
          WaitingProcess(release_[i].Get(Message::ENTRY_FIELD), release_[i].Get(Message::RANK_FIELD))
        );
      }

      communicator_.Irecv(i, &release_[i], Communicator::RELEASE_TAG);
    }
  }
}

void ThiefProcess::Try_release_resources() {
  time_t now;
  time(&now);
  while (!left_houses_queue_.Empty() &&
      left_houses_queue_.Front().Get_expiration_time().Has_expired()) {
    LeftHouse left_house = left_houses_queue_.Front();

    LOG_INFO("house " << left_house.Get_id() << " released")

    int current_timestamp = Increment_timestamp();

    while (!waiting_houses_queue_[left_house.Get_id()].Empty()) {
      WaitingProcess wp = waiting_houses_queue_[left_house.Get_id()].Top();
     
      Message msg = Message()
        .Set(Message::RANK_FIELD, Get_rank())
        .Set(Message::TIMESTAMP_FIELD, current_timestamp);
      communicator_.Send(wp.Get_rank(), msg, Communicator::CONFIRM_TAG);

      waiting_houses_queue_[left_house.Get_id()].Pop();
    }

    house_entry_timestamp_[left_house.Get_id()] = -1;
    left_houses_queue_.Pop();
  }
}

void ThiefProcess::Partnership_insert() {
  entry_timestamp_ = Increment_timestamp();

  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, entry_timestamp_)
    .Set(Message::QUEUE_FIELD, PARTNERSHIP_Q_ID);

  communicator_.Send_all(msg, Communicator::REQUEST_TAG);

  partnership_queue_.Insert(WaitingProcess(entry_timestamp_, Get_rank()));

  communicator_.Irecv_all(confirm_, Communicator::CONFIRM_TAG);

  state_ = &ThiefProcess::Partnership_wait_for_confirm;
}

void ThiefProcess::Partnership_wait_for_confirm() {
  bool confirmed = communicator_.Test_all(Communicator::CONFIRM_TAG);
  if (confirmed) {
    state_ = &ThiefProcess::Partnership_wait_for_top;
  }
}

void ThiefProcess::Partnership_wait_for_top() {
  if (partnership_queue_.Is_in_top(Get_sizes().Get_number_of_thieves(), WaitingProcess(entry_timestamp_, Get_rank()))) {
    state_ = &ThiefProcess::Partnership_critical_section;
  }
}

void ThiefProcess::Partnership_critical_section() {
  LOG_DEBUG("partnership critical section")

  if (partnership_queue_.Position_of(WaitingProcess(entry_timestamp_, Get_rank())) % 2 == 1) {
    Increment_timestamp();
    communicator_.Irecv(Communicator::ANY_SOURCE,
      &partner_sync_, Communicator::PARTNER_TAG);
    state_ = &ThiefProcess::Partnership_wait_for_partner;
  } else {
    current_partner_rank_ = partnership_queue_.Before(WaitingProcess(entry_timestamp_, Get_rank())).Get_rank();
    state_ = &ThiefProcess::Partnership_notify_partner;
  }
}

void ThiefProcess::Partnership_wait_for_partner() {
  if (communicator_.Test(Communicator::ANY_SOURCE, Communicator::PARTNER_TAG)) {
    Increment_timestamp(partner_sync_.Get(Message::TIMESTAMP_FIELD));
    current_partner_rank_ = partner_sync_.Get(Message::RANK_FIELD);
    LOG_INFO("received partner: " << current_partner_rank_)
    state_ = &ThiefProcess::Partnership_release;
  }
}

void ThiefProcess::Partnership_notify_partner() {
  int current_timestamp = Increment_timestamp();
  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, current_timestamp);
  communicator_.Send(current_partner_rank_, msg, Communicator::PARTNER_TAG);

  LOG_INFO("found partner: " << current_partner_rank_)
  state_ = &ThiefProcess::Docs_start_waiting_for_partner;
}

void ThiefProcess::Partnership_release() {
  assert(Get_rank() != current_partner_rank_);
  assert(current_partner_rank_ > -1);

  int current_timestamp = Increment_timestamp();

  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, current_timestamp)
    .Set(Message::QUEUE_FIELD, PARTNERSHIP_Q_ID)
    .Set(Message::ENTRY_FIELD, entry_timestamp_);

  communicator_.Send_all(msg, Communicator::RELEASE_TAG);

  partnership_queue_.Erase(
    partnership_queue_.After(WaitingProcess(entry_timestamp_, Get_rank()))
  );
  partnership_queue_.Erase(
    WaitingProcess(entry_timestamp_, Get_rank())
  );

  state_ = &ThiefProcess::Docs_request_entry;
}

void ThiefProcess::Docs_request_entry() {
  entry_timestamp_ = Increment_timestamp();

  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, entry_timestamp_)
    .Set(Message::QUEUE_FIELD, DOCUMENTATION_Q_ID);

  communicator_.Send_all(msg, Communicator::REQUEST_TAG);

  documentation_queue_.Insert(WaitingProcess(entry_timestamp_, Get_rank()));

  communicator_.Irecv_all(confirm_, Communicator::CONFIRM_TAG);

  state_ = &ThiefProcess::Docs_wait_for_confirm;
}

void ThiefProcess::Docs_wait_for_confirm() {
  bool confirmed = communicator_.Test_all(Communicator::CONFIRM_TAG);
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
  if (!sleep_start_.Is_initialized()) {
    sleep_start_ = TimePoint::Now();
    LOG_INFO("has started filling the docs (critical section)")
  } else {
    TimePoint paperwork_end = sleep_start_ + Duration(PAPERWORK_DURATION);
    if (paperwork_end.Has_expired()) {
      LOG_INFO("has finished filling the docs")
      sleep_start_.Reset();
      state_ = &ThiefProcess::Docs_release;
    }
  }
}

void ThiefProcess::Docs_release() {

  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank());
  communicator_.Send(current_partner_rank_, msg, Communicator::PARTNER_TAG);

  Increment_timestamp();

  msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, entry_timestamp_)
    .Set(Message::QUEUE_FIELD, DOCUMENTATION_Q_ID)
    .Set(Message::ENTRY_FIELD, entry_timestamp_);

  communicator_.Send_all(msg, Communicator::RELEASE_TAG);

  documentation_queue_.Erase(WaitingProcess(entry_timestamp_, Get_rank()));

  LOG_INFO("has started waiting for finishing burglary")
  communicator_.Irecv(current_partner_rank_,
    &partner_sync_, Communicator::PARTNER_TAG);
  state_ = &ThiefProcess::House_wait_for_partner;
}

void ThiefProcess::Docs_start_waiting_for_partner() {
  LOG_INFO("has started waiting for the docs")
  communicator_.Irecv(Communicator::ANY_SOURCE,
    &partner_sync_, Communicator::PARTNER_TAG);
  state_ = &ThiefProcess::Docs_wait_for_partner;
}

void ThiefProcess::Docs_wait_for_partner() {
  if (communicator_.Test(Communicator::ANY_SOURCE, Communicator::PARTNER_TAG)) {
    LOG_INFO(current_partner_rank_ << " - has filled the docs")
    state_ = &ThiefProcess::House_request_entry;
  }
}

void ThiefProcess::House_request_entry() {
  const int house_id = 0;  // TODO change it
  if (house_entry_timestamp_[house_id] == -1) {
    LOG_DEBUG("requests entry to " << house_id << " house")
    house_entry_timestamp_[house_id] = Increment_timestamp();

    Message msg = Message()
      .Set(Message::RANK_FIELD, Get_rank())
      .Set(Message::TIMESTAMP_FIELD, house_entry_timestamp_[house_id])
      .Set(Message::QUEUE_FIELD, HOUSE_Q_ID+house_id)
      .Set(Message::ENTRY_FIELD, house_entry_timestamp_[house_id]);

    communicator_.Send_all(msg, Communicator::REQUEST_TAG);
    communicator_.Irecv_all(confirm_, Communicator::CONFIRM_TAG);

    state_ = &ThiefProcess::House_wait_for_confirm;
  }
}

void ThiefProcess::House_wait_for_confirm() {
  bool confirmed = communicator_.Test_all(Communicator::CONFIRM_TAG);
  if (confirmed) {
    state_ = &ThiefProcess::House_critical_section;
  }
}

void ThiefProcess::House_critical_section() {
  const int house_id = 0;
  if (!sleep_start_.Is_initialized()) {
    sleep_start_ = TimePoint::Now();
    LOG_INFO("has started robbing the house (critical section)")
  } else {
    TimePoint burglary_end = sleep_start_ + Duration(BURGLARY_DURATION);
    if (burglary_end.Has_expired()) {
      LOG_INFO("has finished robbing the house")
      TimePoint expiration_time = TimePoint::Now() + Duration(1);
      left_houses_queue_.Push(LeftHouse(house_id, expiration_time));
      sleep_start_ = TimePoint::Now();
      state_ = &ThiefProcess::House_notify_partner;
    }
  }
}

void ThiefProcess::House_wait_for_partner() {
  if (communicator_.Test(current_partner_rank_, Communicator::PARTNER_TAG)) {
    Increment_timestamp(partner_sync_.Get(Message::TIMESTAMP_FIELD));
    LOG_INFO("received house info")
    state_ = &ThiefProcess::Partnership_insert;
  }
}

void ThiefProcess::House_notify_partner() {
  int current_timestamp = Increment_timestamp();
  Message msg = Message()
    .Set(Message::RANK_FIELD, Get_rank())
    .Set(Message::TIMESTAMP_FIELD, current_timestamp);

  communicator_.Send(current_partner_rank_, msg, Communicator::PARTNER_TAG);

  state_ = &ThiefProcess::Partnership_insert;
}

int ThiefProcess::Increment_timestamp(int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}

