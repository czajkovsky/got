#include "waiting_process.h"

#include <ostream>
#include <algorithm>

WaitingProcess::WaitingProcess(int timestamp, int rank)
  : timestamp_(timestamp), rank_(rank) {
}

WaitingProcess::WaitingProcess(const WaitingProcess& wp)
  : timestamp_(wp.timestamp_), rank_(wp.rank_) {
}

WaitingProcess& WaitingProcess::operator=(WaitingProcess wp) {
  std::swap(timestamp_, wp.timestamp_);
  std::swap(rank_, wp.rank_);
  return *this;
}

bool operator<(const WaitingProcess& a, const WaitingProcess& b) {
  if (a.timestamp_ < b.timestamp_) {
    return true;
  } else if (a.timestamp_ > b.timestamp_) {
    return false;
  } else {
    return a.rank_ < b.rank_;
  }
}

bool operator==(const WaitingProcess& a, const WaitingProcess& b) {
  return ((a.timestamp_ == b.timestamp_) && (a.rank_ == b.rank_));
}

std::ostream& operator<<(std::ostream& out, const WaitingProcess& wp) {
  return out << "[ timestamp = " << wp.timestamp_ << ", rank = " << wp.rank_ << " ]";
}
