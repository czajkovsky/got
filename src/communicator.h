#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <mpi.h>
#include "message.h"

class Communicator {
public:
  Communicator() {}

  enum CommunicationTAG {
    REQUEST_TAG,
    CONFIRM_TAG,
    RELEASE_TAG,
    PARTNER_TAG,
    DOCS_TAG,
    HOUSE_TAG
  };

  MPI::Request Irecv(int source, Message*, CommunicationTAG);
  MPI::Request* Irecv_all(Message*, CommunicationTAG, MPI::Request*);

  static int Get_rank();
  static int Get_size();

private:
  Communicator(const Communicator&);
  Communicator& operator=(const Communicator&);

};

#endif  // COMMUNICATOR_H_
