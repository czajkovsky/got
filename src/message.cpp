#include "message.h"

#include <ostream>
#include <algorithm>

Message::Message() {
  std::fill_n(message_, MESSAGE_LENGTH, -1);
}

Message::Message(const Message& m) {
  std::copy(m.message_, m.message_+MESSAGE_LENGTH, message_);
}

Message& Message::operator=(Message m) {
  std::swap_ranges(m.message_, m.message_+MESSAGE_LENGTH, message_);
  return *this;
}

Message& Message::Set(MessageField field, int value) {
  message_[field] = value;
  return *this;
}

int* Message::To_array() {
  return message_;
}

std::ostream& operator<<(std::ostream& out, const Message& m) {
  out << "[ "
      << m.Get(Message::RANK_FIELD) << ", "
      << m.Get(Message::TIMESTAMP_FIELD) << ", "
      << m.Get(Message::QUEUE_FIELD) << ", "
      << m.Get(Message::ENTRY_FIELD) << " ]";
  return out;
}
