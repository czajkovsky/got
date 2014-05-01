#ifndef WAITING_QUEUE_H_
#define WAITING_QUEUE_H_

#include "waiting_process.h"
#include <set>

class WaitingQueue {
public:
  WaitingQueue() {}

  WaitingProcess Insert(const WaitingProcess&);

  unsigned int Position_of(const WaitingProcess&);

  bool Is_on_top(const WaitingProcess&) const;
  bool Is_in_top(unsigned int k, const WaitingProcess&) const;

  bool Pop();
  bool Erase(const WaitingProcess&);

  void Print() const;

private:
  WaitingQueue(const WaitingQueue&);
  WaitingQueue& operator=(const WaitingQueue&);

  std::set<WaitingProcess> collection_;
};

#endif  // WAITING_QUEUE_H_
