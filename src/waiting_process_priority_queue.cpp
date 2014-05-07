#include "waiting_process_priority_queue.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cassert>

WaitingProcess WaitingProcessPriorityQueue::Insert(const WaitingProcess& wp) {
  collection_.insert(wp);
  return wp;
}

unsigned int WaitingProcessPriorityQueue::Position_of(const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    return static_cast<unsigned int>(std::distance(collection_.begin(), found)) + 1;
  } else {
    return 0;
  } 
}

WaitingProcess WaitingProcessPriorityQueue::Before(const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end() && found != collection_.begin()) {
    found--;
    return *found;
  } else {
    return wp;
  }
}

WaitingProcess WaitingProcessPriorityQueue::After(const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    found++;
    if (found != collection_.end()) {
      return *found;
    } else {
      return wp;
    }
  } else {
    return wp;
  }
}

bool WaitingProcessPriorityQueue::Is_on_top(const WaitingProcess& wp) const {
  if (collection_.empty()) {
    return false;
  } else {
    std::set<WaitingProcess>::iterator top = collection_.begin();
    return (*top == wp);
  }
}

bool WaitingProcessPriorityQueue::Is_in_top(unsigned int k, const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    return (std::distance(collection_.begin(), found) < k);
  } else {
    return false;
  }
}

bool WaitingProcessPriorityQueue::Pop() {
  if (collection_.empty()) {
    return false;
  }
  std::set<WaitingProcess>::iterator top = collection_.begin();
  collection_.erase(top);
  return true;
}

bool WaitingProcessPriorityQueue::Erase(const WaitingProcess& wp) {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    collection_.erase(found);
    return true;
  } else {
    return false;
  }
}

bool WaitingProcessPriorityQueue::Empty() {
  return collection_.empty();
}

WaitingProcess WaitingProcessPriorityQueue::Top() {
  assert(!Empty());
  return *(collection_.begin());
}

void WaitingProcessPriorityQueue::Print() const {
  std::copy(collection_.begin(), collection_.end(), std::ostream_iterator<WaitingProcess>(std::cout, "\n"));
}
