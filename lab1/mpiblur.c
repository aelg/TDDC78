#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include "ppmio.h"
#include "mpiblurfilter.h"
#include "gaussw.h"

#define MAX_RAD 1000

int max(int a, int b){return a>b?a:b;}
int min(int a, int b){return a>b?b:a;}

int main (argc, argv)
  int argc;
  char *argv[];
{
  int rank, size;
  int y, i, blocksize, overlapping_lines, *recvcounts, l_ysize, *displs;
  int *blockstart;
  int linelength;
  int radius, xsize, ysize, colmax;
  double w[MAX_RAD], start_time = 0, end_time, float_ops, run_time;
  char *src = 0, *l_src = 0;
  MPI_Status status;
  MPI_Request *request;

  MPI_Init (&argc, &argv);  /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);  /* get number of processes */

  /* Rank 0 reads file.*/
  if(rank == 0){
    /* Allocate memory dynamically. (Had some problems with static allocation and huge image files.) */
    src = malloc(sizeof(pixel)*MAX_PIXELS);
    if(!src){
      printf("Could not allocate memory, exiting");
      exit(1);
    }

    /* Take care of the arguments */

    if (argc != 4) {
      fprintf(stderr, "Usage: %s radius infile outfile\n", argv[0]);
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
    printf("Read image.\n");
  }

  /* Start clock for timing of execution. */
  if(rank == 0) start_time = MPI_Wtime();

  /* Distribute some image data. */
  MPI_Bcast(&xsize, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ysize, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
  MPI_Bcast(&radius, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

  /* Calculate offsets for data to be sent to processes. */
  y = 0;
  overlapping_lines = radius;
  linelength = xsize*3;
  blockstart = malloc(sizeof(int)*(size+1));
  recvcounts = malloc(sizeof(int)*size);
  displs = malloc(sizeof(int)*size);
  blocksize = ysize/size;
  for(i = 0; i <= size; ++i){
    blockstart[i] = y;
    if(i != size){
      y += blocksize + (i < ysize%size ? 1:0);
      displs[i] = blockstart[i]*linelength;
      recvcounts[i] = (y-blockstart[i])*linelength;
    }
  }
  MPI_Bcast(blockstart, size+1, MPI_INTEGER, 0, MPI_COMM_WORLD);
  /* blockstart[i] now contains starting line for data to be processed by rank i.
   * blockstart[i+1] contains the line after the last line in data to be processed by rank i.*/
  /* recvcounts[] and displs[] are used by MPI_Gatherv after processing. */

  /* Allocate data for the request array to be able to wait for all send operations.*/
  request = malloc(sizeof(MPI_Request)*(size-1));

  /* Send image data to all processes. */
  if(rank == 0){
    char *datastart, *dataend;
    printf("Distributing image data to processes\n");

    for(i = 1; i < size; ++i){
      datastart = src+max(0, blockstart[i]-overlapping_lines)*linelength;
      dataend = src+min(ysize, blockstart[i+1]+overlapping_lines)*linelength;
      MPI_Isend(datastart, dataend-datastart, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[i-1]);
    }

    /* Copy image data of rank 0 to local buffer */
    l_src = malloc((blockstart[1]+overlapping_lines)*linelength);
    memcpy(l_src, src, (blockstart[1]+overlapping_lines)*linelength);
  }

  /* Recieve data at all nodes except the first one. */
  if(rank > 0){
    l_ysize = blockstart[rank+1] - blockstart[rank];
    l_src = malloc((l_ysize + overlapping_lines*2)*linelength);
    MPI_Recv(l_src, (l_ysize + overlapping_lines*2)*linelength, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
  }

  /* Generate coefficients for the filter*/
  get_gauss_weights(radius, w);
  
  /* Move image data pointer to where the processing should begin.*/
  l_src += min(overlapping_lines, blockstart[rank])*linelength;

  if(rank == 0) printf("Running filter.\n");

  /* Run filter. */
  blurfilter(l_src, xsize, ysize, blockstart[rank], blockstart[rank+1], w, radius);

  /* Wait for all processes to complete the filtering before calling Gatherv().
   * There could possibly be some writing to src before all data is sent if this is not done.*/
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0){
    printf("All filtering is complete.\n");
  }
  /* Gather all data from the processes. */
  MPI_Gatherv(l_src, recvcounts[rank], MPI_CHAR, src, recvcounts, displs, MPI_CHAR, 0, MPI_COMM_WORLD);

  if(rank == 0){
    end_time = MPI_Wtime();
    printf("Writing data to disk.\n");
    if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0)
      exit(1);

    run_time = end_time - start_time;
    float_ops = (double)xsize*ysize*(14*radius+7)*2;
    printf("Filtering took: %g secs\n", run_time);
    /*printf("# floating point operations ~ %.0f\n", float_ops);*/
    printf("MFLOPS ~ %.2f\n", float_ops/(run_time*1000000));
  }
  
  free(l_src - min(overlapping_lines, blockstart[rank])*linelength);
  free(blockstart);
  free(recvcounts);
  free(displs);
  free(request);
  free(src);

  MPI_Finalize();
  return 0;
}

