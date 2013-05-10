#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <inttypes.h>
#include "ppmio.h"

#define MAX_RAD 1000

int max(int a, int b){return a>b?a:b;}
int min(int a, int b){return a>b?b:a;}

int main (argc, argv)
  int argc;
  char *argv[];
{
  int rank, nprocs, i, blocksize, xsize, ysize, 
      colmax, *recvcounts, *displs, num_pixels;
  double sum = 0, average = 0;
  double start_time = 0, end_time, run_time;
  unsigned char *src = 0, *l_src = 0;


  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);  /* get number of processes */
  

  /* Rank 0 reads file.*/
  if(rank == 0){
    /* Allocate memory dynamically. 
     * (Had some problems with static allocation and huge image files.) */
    src = malloc(3*MAX_PIXELS);
    if(!src){
      printf("Could not allocate memory, exiting");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* Take care of the arguments */

    if (argc != 3) {
      fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* read file */
    if(read_ppm (argv[1], &xsize, &ysize, &colmax, (char *) src) != 0)
      MPI_Abort(MPI_COMM_WORLD, 1);

    if (colmax > 255) {
      fprintf(stderr, "Too large maximum color-component value\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    printf("Read image.\n");
  }

  /* Start clock for timing of execution. */
  if(rank == 0) start_time = MPI_Wtime();

  /* Distribute some image data. */
  MPI_Bcast(&xsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ysize, 1, MPI_INT, 0, MPI_COMM_WORLD);

  /* Calculate offsets for data to be sent to processes. */
  recvcounts = malloc(sizeof(int)*nprocs);
  displs = malloc(sizeof(int)*nprocs);
  blocksize = (ysize*xsize*3)/nprocs;
  displs[0] = 0;
  for(i = 0; i < nprocs; ++i){
    recvcounts[i] = blocksize + (i < (ysize*xsize*3)%nprocs ? 3:0);
    if(i > 0) displs[i] = displs[i-1] + recvcounts[i];
  }

  /* Allocate local buffer. */
  l_src = malloc(recvcounts[rank]);
  /* Send image data to all processes. */
  MPI_Scatterv(src, recvcounts, displs, MPI_UNSIGNED_CHAR, 
      l_src, recvcounts[rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);


  if(rank == 0) printf("Running filter.\n");

  /* Calculate local sum.*/
  for(i = 0; i < recvcounts[rank]; ++i){
    sum += l_src[i];
  }

  /* Calculate global sum and average. */
  MPI_Allreduce(&sum, &average, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  average = average/(xsize*ysize);

  if(rank == 0) printf("First pass done.\nStarting second pass.\n");

  /* Threshold. */
  for(i = 0; i < recvcounts[rank]; i += 3){
    if (l_src[i] + l_src[i+1] + l_src[i+2] > average)
      l_src[i] = l_src[i+1] = l_src[i+2] = 255;
    else
      l_src[i] = l_src[i+1] = l_src[i+2] = 0;
  }

  if(rank == 0){
    printf("All filtering is complete.\n");
  }

  /* Gather all data from the processes. */
  MPI_Gatherv(l_src, recvcounts[rank], MPI_UNSIGNED_CHAR, 
      src, recvcounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  if(rank == 0){
    end_time = MPI_Wtime();
    printf("Writing data to disk.\n");
    if(write_ppm (argv[2], xsize, ysize, (char *)src) != 0)
      MPI_Abort(MPI_COMM_WORLD, 1);

    run_time = end_time - start_time;
    num_pixels = (double)xsize*ysize;
    printf("Filtering took: %g secs\n", run_time);
    /*printf("# floating point operations ~ %.0f\n", float_ops);*/
    printf("MPixels/second ~ %.2f\n", num_pixels/(run_time*1000000));
  }

  MPI_Finalize();
  return 0;
}

