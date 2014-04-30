#ifndef WAITING_PROCESS_H_
#define WAITING_PROCESS_H_

#include <ostream>

class WaitingProcess {
public:
  WaitingProcess(unsigned int timestamp, int rank);
  WaitingProcess(const WaitingProcess&);
  WaitingProcess& operator=(WaitingProcess);

  friend bool operator<(const WaitingProcess&, const WaitingProcess&);
  friend bool operator==(const WaitingProcess&, const WaitingProcess&);
  friend std::ostream& operator<<(std::ostream&, const WaitingProcess&);

private:
  unsigned int timestamp_;
  int rank_;
};

#endif  // WAITING_PROCESS_H_
