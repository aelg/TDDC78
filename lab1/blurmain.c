#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"

#define NUM_THREADS 10

int main (int argc, char ** argv) {
  int radius;
  int xsize, ysize, colmax;
  pixel src[MAX_PIXELS], dst[MAX_PIXELS];
  struct timespec stime, etime;
  struct blurfilter_attr threads_params[NUM_THREADS];
  long thread_ids[NUM_THREADS];

#define MAX_RAD 1000

  double w[MAX_RAD];

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

  printf("Has read the image, generating coefficients\n");

  /* filter */
  get_gauss_weights(radius, w);

  printf("Calling filter\n");

  clock_gettime(CLOCK_REALTIME, &stime);

  int y = 0, x = 0;
  int i;
  for(i = 0; i < NUM_THREADS; ++i){
    // Fill data for the thread.
    threads_params[i].xsize = xsize;
    threads_params[i].ysize = ysize;
    threads_params[i].src = src;
    threads_params[i].dst = dst;
    threads_params[i].w = w;
    threads_params[i].radius = radius;

    // Calculate the region that the thread should operate on.
    threads_params[i].starty = y;
    y += ysize/NUM_THREADS;
    if(i < ysize%NUM_THREADS) ++y;
    threads_params[i].stopy = y;
    
    // Start thread
    pthread_create(&thread_ids[i], NULL, blurfilter, (void*) &threads_params[i]);
  }
  // Wait for all threads to finish the first pass.
  for(i = 0; i < NUM_THREADS; ++i){
    void *status;
    pthread_join(thread_ids[i], &status);
  }
  // After first pass the picture will be transposed and stored in dst.
  // Note the exchange of x and y everywhere.
  for(i = 0;  i < NUM_THREADS; ++i){
    threads_params[i].xsize = ysize;
    threads_params[i].ysize = xsize;
    threads_params[i].src = dst;
    threads_params[i].dst = src;

    threads_params[i].starty = x;
    x += xsize/NUM_THREADS;
    if(i < xsize%NUM_THREADS) ++x;
    threads_params[i].stopy = x;

    pthread_create(&thread_ids[i], NULL, blurfilter, (void*) &threads_params[i]);
  }
  // Wait for all threads to terminate.
  for(i = 0; i < NUM_THREADS; ++i){
    void *status;
    pthread_join(thread_ids[i], &status);
  }

  clock_gettime(CLOCK_REALTIME, &etime);

  printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
      1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

  /* write result */
  printf("Writing output file\n");

  if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0)
    exit(1);


  return(0);
}
