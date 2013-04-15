#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"

#define MAX_RAD 1000

int main (argc, argv)
  int argc;
  char *argv[];
{
  int *sendcounts, *displs;
  int rank, size;
  int y, i, blocksize;
  pixel *src, *dst;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */

  /* Rank 0 reads file.*/
  if(rank == 0){
    src = malloc(sizeof(pixel)*MAX_PIXELS);
    dst = malloc(sizeof(pixel)*MAX_PIXELS);
    if(!src || !dst){
      printf("Could not allocate memory, exiting");
      exit(1);
    }

    sendcounts = malloc(sizeof(int)*size);
    displs = malloc(sizeof(int)*size);

    /* Take care of the arguments */

    if (argc != 4) {
      fprintf(stderr, "Usage: %s threads radius infile outfile\n", argv[0]);
      exit(1);
    }
    radius = atoi(argv[1]);
    if((radius > MAX_RAD) || (radius < 1)) {
      fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
      exit(1);
    }

    /* read file */
    if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
      exit(1);

    if (colmax > 255) {
      fprintf(stderr, "Too large maximum color-component value\n");
      exit(1);
    }

    /*printf("Has read the image, generating coefficients\n");*/

    /* filter */
    get_gauss_weights(radius, w);

    /* Calculate offsets for data to be sent to processes. */
    y = 0;
    blocksize = ysize/size;
    for(i = 0; i < size; ++i){
      sendcounts[i] = blocksize + (i < ysize%size ? 1:0);
      displs[i] = y;
      y += sendcounts[i];
    if(i < ysize%num_threads) ++y;
    threads_params[i].stopy = y;



  MPI_Finalize();
  return 0;
}

