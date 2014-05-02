#include "waiting_queue.h"

#include <iostream>
#include <algorithm>
#include <iterator>

WaitingProcess WaitingQueue::Insert(const WaitingProcess& wp) {
  collection_.insert(wp);
  return wp;
}

unsigned int WaitingQueue::Position_of(const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    return static_cast<unsigned int>(std::distance(collection_.begin(), found)) + 1;
  } else {
    return 0;
  } 
}

WaitingProcess WaitingQueue::Before(const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end() && found != collection_.begin()) {
    found--;
    return *found;
  } else {
    return wp;
  }
}

bool WaitingQueue::Is_on_top(const WaitingProcess& wp) const {
  if (collection_.empty()) {
    return false;
  } else {
    std::set<WaitingProcess>::iterator top = collection_.begin();
    return (*top == wp);
  }
}

bool WaitingQueue::Is_in_top(unsigned int k, const WaitingProcess& wp) const {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    return (std::distance(collection_.begin(), found) < k);
  } else {
    return false;
  }
}

bool WaitingQueue::Pop() {
  if (collection_.empty()) {
    return false;
  }
  std::set<WaitingProcess>::iterator top = collection_.begin();
  collection_.erase(top);
  return true;
}

bool WaitingQueue::Erase(const WaitingProcess& wp) {
  std::set<WaitingProcess>::iterator found = collection_.find(wp);
  if (found != collection_.end()) {
    collection_.erase(found);
    return true;
  } else {
    return false;
  }
}

void WaitingQueue::Print() const {
  std::copy(collection_.begin(), collection_.end(), std::ostream_iterator<WaitingProcess>(std::cout, "\n"));
}
