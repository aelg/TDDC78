#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define SIZE 1002

inline double max(double a, double b){return ((a < b)?b:a);}

void print_res(double(*T)[SIZE]){
  int i, j;
  for(i = 0; i < SIZE; ++i){
    for(j = 0; j < SIZE; ++j){
      printf("%f ", T[i][j]);
    }
    printf("\n");
  }
}

void save_res(char *filename, double(*T)[SIZE]){
  FILE *file;
  int i, j;
  file = fopen(filename, "w");
  for(i = 0; i < SIZE; ++i){
    for(j = 0; j < SIZE; ++j){
      fprintf(file, "%10.6f", T[i][j]);
    }
    fprintf(file,"\n");
  }
}


int const n = SIZE;
int const N = SIZE*SIZE;
int const maxiter = 1000;
int main(){
  double tol = 1.0e-3;
  double (*T)[SIZE] = malloc(N*sizeof(double)/sizeof(char));
  double *Ttemp = malloc(n*sizeof(double)/sizeof(char));
  double *prev;
  double *ptr, *end_ptr;
  double error;
  double execTime;
  struct timespec tStart, tEnd;
  int k, i, j, rank, numThreads;

  /* Initialize */
  ptr = (double*)T;
  end_ptr = ptr + N;
  for(; ptr != end_ptr; ++ptr){
      *ptr = 0.0;
  }
  for(i = 0; i < n; ++i)
    T[i][0] = 1.0;
  for(i = 0; i < n; ++i)
    T[i][n-1] = 1.0;
  for(i = 0; i < n; ++i)
    T[n-1][i] = 2.0;
  /*print_res(Ttemp);*/

  /* Get time */
  clock_gettime(CLOCK_REALTIME, &tStart);

  /* Run calculation */
  error = 1e9;
  for(k = 0; k < maxiter && error >= tol; ++k){
    error = 0;
    for(i = 1; i < n-1; ++i){
#pragma omp parallel reduction(max :error), private(i, j, stop)
      {
        int stop;
        rank = omp_get_thread_num();
        numThreads = omp_get_num_threads();
        prev = malloc(numThreads*sizeof(int)/sizeof(char));
        stop = 1+(rank+1)*n/NumThreads;
        for(j = 1+rank*n/numThreads; j < stop-2; ++j){
          prev = T[i][j];
          T[i][j] = (T[i-1][j] + T[i+1][j] + T[i][j+1] + T[i][j-1])/4.0;
          error = max(error, fabs(T[i][j] - prev));
          tmp = prev;
        }
        free(prev);
      }
    }
  }

  /* Get time */
  clock_gettime(CLOCK_REALTIME, &tEnd);

  /* Print */
  execTime = (tEnd.tv_sec  - tStart.tv_sec) +
    1e-9*(tEnd.tv_nsec  - tStart.tv_nsec);
  printf("Time: %f   Number of iterations: %d\n", execTime, k);
  printf("Temperature of element T(1,1) = %.17f\n", T[1][1]);

  /* Save to file. */
  save_res("result_c.dat", T);

  return 0;
}

