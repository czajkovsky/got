#ifndef TIME_POINT_H_
#define TIME_POINT_H_

#include <ctime>
#include <ostream>

typedef time_t Duration;

class TimePoint {
public:
  TimePoint(const TimePoint&);
  TimePoint& operator=(TimePoint);

  static TimePoint Now();
  static TimePoint Uninitialized();

  void Reset();

  bool Is_initialized() const;
  bool Has_expired() const;

  TimePoint& operator+=(Duration);
  friend TimePoint operator+(const TimePoint&, Duration);

  bool operator==(const TimePoint&) const;
  bool operator!=(const TimePoint&) const;
  bool operator<(const TimePoint&) const;
  bool operator<=(const TimePoint&) const;
  bool operator>(const TimePoint&) const;
  bool operator>=(const TimePoint&) const;

  friend std::ostream& operator<<(std::ostream&, const TimePoint&);

private:
  TimePoint(time_t time_point);

  time_t time_point_;
};

#endif  // TIME_POINT_H_
