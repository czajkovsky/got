#include "communicator.h"

#include <mpi.h>
#include "message.h"

MPI::Request Communicator::Irecv(int source, Message* msg, CommunicationTAG tag) {
  MPI::Request request;

  request = MPI::COMM_WORLD.Irecv(
    msg->To_array(), Message::MESSAGE_LENGTH, MPI_INT, source, tag);

  return request;
}

MPI::Request* Communicator::Irecv_all(Message* msgs, CommunicationTAG tag,
    MPI::Request* requests) {
  for (unsigned int i=0; i<Get_size(); i++) {
    if (i == Get_rank()) continue;

    requests[i] = Irecv(i, &msgs[i], tag);
  }

  return requests;
}

int Communicator::Get_rank() {
  static int rank = MPI::COMM_WORLD.Get_rank();
  return rank;
}

int Communicator::Get_size() {
  static int size = MPI::COMM_WORLD.Get_size();
  return size;
}
