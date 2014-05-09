#ifndef SIZES_H_
#define SIZES_H_

#include <cassert>
#include <algorithm>

class Sizes {
public:
  Sizes(int number_of_thieves, int number_of_desks, int number_of_houses)
    : number_of_thieves_(number_of_thieves),
      number_of_desks_(number_of_desks),
      number_of_houses_(number_of_houses) {
    assert(number_of_thieves_ <= MAX_NUMBER_OF_THIEVES);
    assert(number_of_desks_ <= MAX_NUMBER_OF_DESKS);
    assert(number_of_houses_ <= MAX_NUMBER_OF_HOUSES);
  }

  Sizes()
    : number_of_thieves_(0),
      number_of_desks_(0),
      number_of_houses_(0) {
  }

  Sizes(const Sizes& sizes)
    : number_of_thieves_(sizes.number_of_thieves_),
      number_of_desks_(sizes.number_of_desks_),
      number_of_houses_(sizes.number_of_houses_) {
  }

  Sizes& operator=(Sizes sizes) {
    std::swap(number_of_thieves_, sizes.number_of_thieves_);
    std::swap(number_of_desks_, sizes.number_of_desks_);
    std::swap(number_of_houses_, sizes.number_of_houses_);
    return *this;
  }

  int Get_number_of_thieves() { return number_of_thieves_; }
  int Get_number_of_desks() { return number_of_desks_; }
  int Get_number_of_houses() { return number_of_houses_; }

  static const int MAX_NUMBER_OF_THIEVES = 10;
  static const int MAX_NUMBER_OF_DESKS = 100;
  static const int MAX_NUMBER_OF_HOUSES = 100;

private:
  int number_of_thieves_;
  int number_of_desks_;
  int number_of_houses_;
};

#endif  // SIZES_H_
