#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "thresfilter.h"
#include <pthread.h>

#define MAX_THREADS 100

int main (int argc, char ** argv) {
  int xsize, ysize, colmax, num_threads;
  int i,equal_work_unit,nump;
  pixel *src = malloc(sizeof(pixel)*MAX_PIXELS);
  struct timespec stime, etime;
  struct thresfilter_attr threads_params[MAX_THREADS];
  unsigned long int thread_ids[MAX_THREADS];
  double run_time, num_pixels;

  pthread_mutex_t sum_lock;	
  int64_t sum = 0;

  pthread_mutex_init(&sum_lock, NULL);

  if(!src){
    printf("Could not allocate memory, exiting");
    exit(1);
  }

  /* Take care of the arguments */

  if (argc != 4) {
    fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
    exit(1);
  }

  /* read file */
  if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
    exit(1);

  /* thread n arg */
  num_threads = atoi(argv[1]);

  if (colmax > 255) {
    fprintf(stderr, "Too large maximum color-component value\n");
    exit(1);
  }

  printf("Read image.\n");

  /* Start clock when image is read.*/
  clock_gettime(CLOCK_REALTIME, &stime);

  printf("Starting threads for first pass.\n");
  /* thread input */
  nump = xsize*ysize;
  equal_work_unit = nump/num_threads;
  for (i = 0; i < num_threads-1; ++i) {
    threads_params[i].start = src + equal_work_unit*i;
    threads_params[i].end = threads_params[i].start + equal_work_unit;
    threads_params[i].sum = &sum;
    threads_params[i].lock = &sum_lock;
  }
  threads_params[num_threads-1].start = src + equal_work_unit*(num_threads-1);
  threads_params[num_threads-1].end = src + nump;
  threads_params[num_threads-1].sum = &sum;
  threads_params[num_threads-1].lock = &sum_lock;

  /* calculate sum with threads */
  for (i = 0; i < num_threads; ++i) {
    pthread_create(&thread_ids[i],NULL,
        thresfiltersum,(void*) &threads_params[i]);
  }


  /* wait for sum */
  for (i = 0; i < num_threads; ++i) {
    void* status;
    pthread_join(thread_ids[i], &status);
  }

  /* average intensity of pixels (Attn: reuse of sum variable) */
  sum /= nump;

  printf("First pass done.\nStarting second pass.\n");
  /* calculate output image with threads */
  for (i = 0; i < num_threads; ++i) {
    pthread_create(&thread_ids[i],NULL,
        thresfilteravg,(void*) &threads_params[i]);
  }

  /* wait for threads to finish */
  for (i = 0; i < num_threads; ++i) {
    void* status;
    pthread_join(thread_ids[i], &status);
  }

  /* Stop clock before writing to disk. */
  clock_gettime(CLOCK_REALTIME, &etime);
  printf("All filtering is complete.\n");

  /* write result */
  printf("Writing data to disk.\n");

  if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0)
    exit(1);

  run_time = (etime.tv_sec  - stime.tv_sec) 
    + 1e-9*(etime.tv_nsec  - stime.tv_nsec);
  num_pixels = (double)xsize*ysize;
  printf("Filtering took: %g secs\n", run_time);
  /*printf("# floating point operations ~ %.0f\n", float_ops);*/
  printf("MPixels/second ~ %.2f\n", num_pixels/(run_time*1000000));


  return(0);
}
