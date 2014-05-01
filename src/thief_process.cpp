#include "thief_process.h"

#include <iostream>
#include <algorithm>
#include <mpi.h>

#include "sizes.h"

void ThiefProcess::Run(int rank, Sizes sizes) {
  rank_ = rank;
  sizes_ = sizes;

  state_ = &ThiefProcess::Insert;

  std::cout << "Starting ThiefProcess [rank = " << Get_rank() << "] "
    << "[thieves = " << Get_sizes().Get_number_of_thieves()
    << ", desks = " << Get_sizes().Get_number_of_desks()
    << ", houses = " << Get_sizes().Get_number_of_houses()
    << "]" << std::endl;

  Set_up_communication();
  Main_loop();
}

void ThiefProcess::Set_up_communication() {
}

void ThiefProcess::Main_loop() {
  // TODO create loop
  // while (true)
  {
    Try_communication();
    (this->*state_)();
    Try_release();
  }
}

void ThiefProcess::Try_communication() {
}

void ThiefProcess::Try_release() {
}

void ThiefProcess::Insert() {
  std::cout << "Insert()" << std::endl;
}

void ThiefProcess::Wait_for_confirm() {
}

void ThiefProcess::Wait_for_top() {
}

void ThiefProcess::Critical_section() {
}

void ThiefProcess::Release() {
}

unsigned int ThiefProcess::Increment_timestamp(unsigned int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}
