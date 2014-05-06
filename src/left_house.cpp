#include "left_house.h"

#include <ostream>
#include <ctime>
#include <algorithm>

LeftHouse::LeftHouse(unsigned int id, time_t expiration_time)
  : id_(id), expiration_time_(expiration_time) {
}

LeftHouse::LeftHouse(const LeftHouse& lh)
  : id_(lh.id_), expiration_time_(lh.expiration_time_) {
}

LeftHouse& LeftHouse::operator=(LeftHouse lh) {
  std::swap(id_, lh.id_);
  std::swap(expiration_time_, lh.expiration_time_);
  return *this;
}

bool operator==(const LeftHouse& a, const LeftHouse& b) {
  return ((a.id_ == b.id_) && (a.expiration_time_ == b.expiration_time_));
}

std::ostream& operator<<(std::ostream& out, const LeftHouse& lh) {
  return out << "[ id = " << lh.id_ << ", expiration time = " << lh.expiration_time_ << " ]";
}
