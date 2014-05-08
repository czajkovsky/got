#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <ostream>

class Message {
public:
  Message();
  Message(const Message&);
  Message& operator=(Message);

  static const unsigned int MESSAGE_LENGTH = 4;
  enum MessageField {
    RANK_FIELD,
    TIMESTAMP_FIELD,
    QUEUE_FIELD,
    ENTRY_FIELD
  };

  Message& Set(MessageField, int value);
  int Get(MessageField field) const { return message_[field]; }

  int* To_array();

  friend std::ostream& operator<<(std::ostream&, const Message&);

private:
  int message_[MESSAGE_LENGTH];
};

#endif  // MESSAGE_H_
