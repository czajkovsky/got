#include "time_point.h"

#include <ctime>
#include <ostream>
#include <algorithm>
#include <cassert>

TimePoint::TimePoint(time_t time_point) 
  : time_point_(time_point) {
}

TimePoint::TimePoint(const TimePoint& tp)
  : time_point_(tp.time_point_) {
}

TimePoint& TimePoint::operator=(TimePoint tp) {
  std::swap(tp.time_point_, time_point_);
  return *this;
}

TimePoint TimePoint::Now() {
  return TimePoint(time(NULL));
}

TimePoint TimePoint::Uninitialized() {
  return TimePoint(static_cast<time_t>(-1));
}

void TimePoint::Reset() {
  time_point_ = static_cast<time_t>(-1);
}

bool TimePoint::Is_initialized() const {
  return time_point_ != static_cast<time_t>(-1);
}

bool TimePoint::Has_expired() const {
  assert(Is_initialized());
  return TimePoint::Now() >= *this;
}

TimePoint& TimePoint::operator+=(Duration duration) {
  assert(Is_initialized());
  time_point_ += static_cast<time_t>(duration);
  return *this;
}

TimePoint operator+(const TimePoint& tp, Duration duration) {
  assert(tp.Is_initialized());
  TimePoint res(tp);
  res += duration;
  return res;
}

bool TimePoint::operator==(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) == 0.0;
}

bool TimePoint::operator!=(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) != 0.0;
}

bool TimePoint::operator<(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) < 0.0;
}

bool TimePoint::operator<=(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) <= 0.0;
}

bool TimePoint::operator>(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) > 0.0;
}

bool TimePoint::operator>=(const TimePoint& tp) const {
  assert(Is_initialized());
  assert(tp.Is_initialized());
  return difftime(time_point_, tp.time_point_) >= 0.0;
}

std::ostream& operator<<(std::ostream& out, const TimePoint& tp) {
  assert(tp.Is_initialized());

  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&tp.time_point_);
  strftime(buf, sizeof(buf), "%X", &tstruct);

  return out << std::string(buf);
}
