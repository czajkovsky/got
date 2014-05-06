#ifndef LEFT_HOUSE_QUEUE_H_
#define LEFT_HOUSE_QUEUE_H_

#include "left_house.h"
#include <queue>

class LeftHouseQueue {
public:
  LeftHouseQueue() {}

  LeftHouse Front() { return queue_.front(); }
  LeftHouse Back() { return queue_.back(); }

  void Push(const LeftHouse& lh) { queue_.push(lh); }
  void Pop() { queue_.pop(); }

  bool Empty() { return queue_.empty(); }

private:
  LeftHouseQueue(const LeftHouseQueue&);
  LeftHouseQueue& operator=(const LeftHouseQueue&);

  std::queue<LeftHouse> queue_;
};

#endif  // LEFT_HOUSE_QUEUE_H_
