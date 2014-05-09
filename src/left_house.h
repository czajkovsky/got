#ifndef LEFT_HOUSE_H_
#define LEFT_HOUSE_H_

#include <ostream>
#include "time_point.h"

class LeftHouse {
public:
  LeftHouse(int id, TimePoint expiration_time);
  LeftHouse(const LeftHouse&);
  LeftHouse& operator=(LeftHouse);

  friend bool operator==(const LeftHouse&, const LeftHouse&);
  friend std::ostream& operator<<(std::ostream&, const LeftHouse&);

  int Get_id() const { return id_; }
  TimePoint Get_expiration_time() const { return expiration_time_; }

private:
  int id_;
  TimePoint expiration_time_;
};

#endif  // LEFT_HOUSE_H_
