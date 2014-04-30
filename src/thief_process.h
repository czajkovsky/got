#ifndef THIEF_PROCESS_H_
#define THIEF_PROCESS_H_

#include "sizes.h"

class ThiefProcess {
public:
  static ThiefProcess& Get_process() {
    static ThiefProcess process;
    return process;
  }
  void Run(int rank, Sizes sizes);

  unsigned int Increment_timestamp(unsigned int other_timestamp = 0);

  int Get_rank() const { return rank_; }
  Sizes Get_sizes() const { return sizes_; }

private:
  ThiefProcess() {}
  ThiefProcess(const ThiefProcess&);
  ThiefProcess& operator=(const ThiefProcess&);

  unsigned int timestamp_;

  int rank_;
  Sizes sizes_;
};

#endif  // THIEF_PROCESS_H_
