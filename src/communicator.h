#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <mpi.h>
#include "message.h"
#include "sizes.h"

class Communicator {
public:
  Communicator() {}

  enum CommunicationTAG {
    REQUEST_TAG,
    CONFIRM_TAG,
    RELEASE_TAG,
    PARTNER_TAG
  };

  void Irecv(int source, Message*, CommunicationTAG);
  void Irecv_all(Message*, CommunicationTAG);

  bool Test(int source, CommunicationTAG);
  bool Test_all(CommunicationTAG);

  void Send(int dest, Message, CommunicationTAG);
  void Send_all(Message, CommunicationTAG);

  static int Get_rank();
  static int Get_size();

  const static int ANY_SOURCE = MPI::ANY_SOURCE;

private:
  Communicator(const Communicator&);
  Communicator& operator=(const Communicator&);

  MPI::Request* Get_request(int source, CommunicationTAG);

  MPI::Request request_request_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request release_request_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request confirm_request_[Sizes::MAX_NUMBER_OF_THIEVES];
  MPI::Request partner_sync_request_;

};

#endif  // COMMUNICATOR_H_
