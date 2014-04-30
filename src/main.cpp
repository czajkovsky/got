#include <mpi.h>
#include "thief_process.h"

int main(int argc, char* argv[]) {
  MPI::Init(argc, argv);

  ThiefProcess::Get_process().Run(
    MPI::COMM_WORLD.Get_size(), MPI::COMM_WORLD.Get_rank());

  MPI::Finalize();
}
