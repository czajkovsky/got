#include "thief_process.h"

#include <iostream>
#include <algorithm>

void ThiefProcess::Run(int size, int rank) {
  size_ = size;
  rank_ = rank;

  std::cout << "Starting ThiefProcess [size = " << size_ << ", rank = " << rank_ << "]" << std::endl;

}

unsigned int ThiefProcess::Increment_timestamp(unsigned int other_timestamp) {
  timestamp_ = std::max(timestamp_, other_timestamp) + 1;
  return timestamp_;
}
