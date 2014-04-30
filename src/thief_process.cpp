#include "thief_process.h"

#include <iostream>
#include <algorithm>

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

  // TODO prepare communication before Main_loop
  Main_loop();
}

void ThiefProcess::Main_loop() {
  // TODO create loop
  // while (true)
  {
    // TODO try communication here
    (this->*state_)();
    // TODO try other things here
  }
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
