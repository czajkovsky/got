#include <mpi.h>
#include "thief_process.h"
#include "sizes.h"

int main(int argc, char* argv[]) {
  MPI::Init(argc, argv);

  const unsigned int number_of_thieves = MPI::COMM_WORLD.Get_size();
  const unsigned int number_of_desks = 1;
  const unsigned int number_of_houses = 2;

  ThiefProcess::Get_process().Run(
    MPI::COMM_WORLD.Get_rank(), Sizes(number_of_thieves, number_of_desks, number_of_houses));

  MPI::Finalize();
}
