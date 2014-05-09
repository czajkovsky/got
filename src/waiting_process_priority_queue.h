#ifndef WAITING_PROCESS_PRIORITY_QUEUE_H_
#define WAITING_PROCESS_PRIORITY_QUEUE_H_

#include "waiting_process.h"
#include <set>

class WaitingProcessPriorityQueue {
public:
  WaitingProcessPriorityQueue() {}

  WaitingProcess Insert(const WaitingProcess&);

  int Position_of(const WaitingProcess&) const;

  WaitingProcess Before(const WaitingProcess&) const;
  WaitingProcess After(const WaitingProcess&) const;

  bool Is_on_top(const WaitingProcess&) const;
  bool Is_in_top(int k, const WaitingProcess&) const;

  bool Pop();
  bool Erase(const WaitingProcess&);

  bool Empty();
  WaitingProcess Top();

  void Print() const;

private:
  WaitingProcessPriorityQueue(const WaitingProcessPriorityQueue&);
  WaitingProcessPriorityQueue& operator=(const WaitingProcessPriorityQueue&);

  std::set<WaitingProcess> collection_;
};

#endif  // WAITING_PROCESS_PRIORITY_QUEUE_H_
