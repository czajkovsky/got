#ifndef LEFT_HOUSE_H_
#define LEFT_HOUSE_H_

#include <ostream>
#include <ctime>

class LeftHouse {
public:
  LeftHouse(unsigned int id, time_t expiration_time);
  LeftHouse(const LeftHouse&);
  LeftHouse& operator=(LeftHouse);

  friend bool operator==(const LeftHouse&, const LeftHouse&);
  friend std::ostream& operator<<(std::ostream&, const LeftHouse&);

  unsigned int Get_id() const { return id_; }
  time_t Get_expiration_time() const { return expiration_time_; }

private:
  unsigned int id_;
  time_t expiration_time_;
};

#endif  // LEFT_HOUSE_H_
