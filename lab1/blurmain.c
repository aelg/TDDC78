#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"

#define MAX_THREADS 100
#define MAX_RAD 1000

int main (int argc, char ** argv) {
  int radius, num_threads;
  int xsize, ysize, colmax;
  int y, x, i;
  pixel *src = malloc(sizeof(pixel)*MAX_PIXELS), *dst = malloc(sizeof(pixel)*MAX_PIXELS);
  struct timespec stime, etime;
  struct blurfilter_attr threads_params[MAX_THREADS];
  unsigned long int thread_ids[MAX_THREADS];
  double w[MAX_RAD];
  double run_time, float_ops;

  if(!src || !dst){
    printf("Could not allocate memory, exiting");
    exit(1);
  }

  /* Take care of the arguments */

  if (argc != 5) {
    fprintf(stderr, "Usage: %s threads radius infile outfile\n", argv[0]);
    exit(1);
  }
  radius = atoi(argv[2]);
  num_threads = atoi(argv[1]);
  if((radius > MAX_RAD) || (radius < 1)) {
    fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
    exit(1);
  }

  /* read file */
  if(read_ppm (argv[3], &xsize, &ysize, &colmax, (char *) src) != 0)
    exit(1);

  if (colmax > 255) {
    fprintf(stderr, "Too large maximum color-component value\n");
    exit(1);
  }

  /*printf("Has read the image, generating coefficients\n");*/

  /* filter */
  get_gauss_weights(radius, w);

  /*printf("Calling filter\n");*/

  clock_gettime(CLOCK_REALTIME, &stime);

  x = y = 0;
  for(i = 0; i < num_threads; ++i){
    /* Fill data for the thread.*/
    threads_params[i].xsize = xsize;
    threads_params[i].ysize = ysize;
    threads_params[i].src = src;
    threads_params[i].dst = dst;
    threads_params[i].w = w;
    threads_params[i].radius = radius;

    /* Calculate the region that the thread should operate on.*/
    threads_params[i].starty = y;
    y += ysize/num_threads;
    if(i < ysize%num_threads) ++y;
    threads_params[i].stopy = y;
    
    /* Start thread */
    pthread_create(&thread_ids[i], NULL, blurfilter, (void*) &threads_params[i]);
  }
  /* Wait for all threads to finish the first pass. */
  for(i = 0; i < num_threads; ++i){
    void *status;
    pthread_join(thread_ids[i], &status);
  }
  /* After first pass the picture will be transposed and stored in dst.
     Note the exchange of x and y everywhere.*/
  for(i = 0;  i < num_threads; ++i){
    threads_params[i].xsize = ysize;
    threads_params[i].ysize = xsize;
    threads_params[i].src = dst;
    threads_params[i].dst = src;

    threads_params[i].starty = x;
    x += xsize/num_threads;
    if(i < xsize%num_threads) ++x;
    threads_params[i].stopy = x;

    pthread_create(&thread_ids[i], NULL, blurfilter, (void*) &threads_params[i]);
  }
  /* Wait for all threads to terminate.*/
  for(i = 0; i < num_threads; ++i){
    void *status;
    pthread_join(thread_ids[i], &status);
  }

  clock_gettime(CLOCK_REALTIME, &etime);

  run_time = (etime.tv_sec  - stime.tv_sec) + 1e-9*(etime.tv_nsec  - stime.tv_nsec);
  float_ops = (double)xsize*ysize*(14*radius+7)*2;
  printf("Filtering took: %g secs\n", run_time);
  /*printf("# floating point operations ~ %.0f\n", float_ops);*/
  printf("MFLOPS ~ %.2f\n", float_ops/(run_time*1000000));

  /* write result */
  /*printf("Writing output file\n");*/

  if(write_ppm (argv[4], xsize, ysize, (char *)src) != 0)
    exit(1);

  free(src);
  free(dst);

  return(0);
}
