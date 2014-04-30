#include "thief_process.h"

#include <iostream>
#include <algorithm>

#include "sizes.h"

void ThiefProcess::Run(int rank, Sizes sizes) {
  rank_ = rank;
  sizes_ = sizes;

  std::cout << "Starting ThiefProcess [rank = " << Get_rank() << "] "
    << "[thieves = " << Get_sizes().Get_number_of_thieves()
    << ", desks = " << Get_sizes().Get_number_of_desks()
    << ", houses = " << Get_sizes().Get_number_of_houses()
    << "]" << std::endl;

}

unsigned int ThiefProcess::Increment_timestamp(unsigned int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}
