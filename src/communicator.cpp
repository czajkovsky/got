#include "communicator.h"

#include <mpi.h>
#include "message.h"

void Communicator::Irecv(int source, Message* msg, CommunicationTAG tag) {
  MPI::Request* request = Get_request(source, tag);
  *request = MPI::COMM_WORLD.Irecv(
    msg->To_array(), Message::MESSAGE_LENGTH, MPI_INT, source, tag);
}

void Communicator::Irecv_all(Message* msgs, CommunicationTAG tag) {
  for (int i=0; i<Get_size(); i++) {
    if (i == Get_rank()) continue;

    Irecv(i, &msgs[i], tag);
  }
}

bool Communicator::Test(int source, CommunicationTAG tag) {
  MPI::Request* request = Get_request(source, tag);
  return request->Test();
}

bool Communicator::Test_all(CommunicationTAG tag) {
  bool received = true;
  for (int i=0; i<Get_size(); i++) {
    if (i == Get_rank()) continue;
    MPI::Request* request = Get_request(i, tag);
    if (!request->Test()) {
      received = false;
      break;
    }
  }

  return received;
}

void Communicator::Send(int dest, Message msg, CommunicationTAG tag) {
  MPI::COMM_WORLD.Send(
    msg.To_array(), Message::MESSAGE_LENGTH, MPI_INT, dest, tag);
}

void Communicator::Send_all(Message msg, CommunicationTAG tag) {
  for (int i=0; i<Get_size(); i++) {
    if (i == Get_rank()) continue;
    Send(i, msg, tag);
  }
}

int Communicator::Get_rank() {
  static const int rank = MPI::COMM_WORLD.Get_rank();
  return rank;
}

int Communicator::Get_size() {
  static const int size = MPI::COMM_WORLD.Get_size();
  return size;
}

MPI::Request* Communicator::Get_request(int source, CommunicationTAG tag) {
  MPI::Request* request;
  switch (tag) {
  case REQUEST_TAG:
    request = &request_request_[source];
    break;
  case CONFIRM_TAG:
    request = &confirm_request_[source];
    break;
  case RELEASE_TAG:
    request = &release_request_[source];
    break;
  case PARTNER_TAG:
    request = &partner_sync_request_;
  }

  return request;
}
