#include <mpi.h>
#include "thief_process.h"
#include "sizes.h"

int main(int argc, char* argv[]) {
  MPI::Init(argc, argv);

  const int number_of_thieves = MPI::COMM_WORLD.Get_size();
  const int number_of_desks = 4;
  const int number_of_houses = 2;
  Sizes sizes(number_of_thieves, number_of_desks, number_of_houses);

  ThiefProcess process(sizes);
  process.Run();

  MPI::Finalize();
}
