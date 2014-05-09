#ifndef WAITING_PROCESS_H_
#define WAITING_PROCESS_H_

#include <ostream>

class WaitingProcess {
public:
  WaitingProcess(int timestamp, int rank);
  WaitingProcess(const WaitingProcess&);
  WaitingProcess& operator=(WaitingProcess);

  friend bool operator<(const WaitingProcess&, const WaitingProcess&);
  friend bool operator==(const WaitingProcess&, const WaitingProcess&);
  friend std::ostream& operator<<(std::ostream&, const WaitingProcess&);

  int Get_timestamp() const { return timestamp_; }
  int Get_rank() const { return rank_; }

private:
  int timestamp_;
  int rank_;
};

#endif  // WAITING_PROCESS_H_
