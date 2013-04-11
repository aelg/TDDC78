#include <stdio.h>
#include <mpi.h>
#include <string.h>

using namespace std;

int main (int argc, char* argv[])
{
  int rank, size;
  char s[6] = "";

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */
  if(rank == 0){
    strcpy(s, "hello");
  }
  MPI_Bcast(s, 6, MPI_CHAR, 0, MPI_COMM_WORLD);
  printf( "Hello world from process %d of %d\ns = %s\n", rank, size, s);
  MPI_Finalize();
  return 0;
}

