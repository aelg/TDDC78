#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

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
  double *prev = malloc(n*sizeof(double)/sizeof(char));
  double *ptr, *end_ptr;
  double error;
  double execTime;
  struct timespec tStart, tEnd;
  int k, i, j;

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
  /*print_res(T);*/

  /* Get time */
  clock_gettime(CLOCK_REALTIME, &tStart);

  /* Run calculation */
  error = 1e9;
  for(k = 0; k < maxiter && error > tol; ++k){
    error = 0;
    memcpy(prev, T[0], n*sizeof(double)/sizeof(char));

    for(i = 1; i < n-1; ++i){
      for(j = 1; j < n-1; ++j){
        double tmp = prev[j];
        prev[j] = (T[i-1][j] + T[i+1][j] + T[i][j+1] + T[i][j-1])/4.0;
        T[i-1][j] = tmp;
        error = max(error, fabs(T[i][j] - prev[j]));
      }
    }
    /*printf("error: %f\n", error);*/

    memcpy(T[n-2], prev, n*sizeof(double)/sizeof(char));
  }

  /* Get time */
  clock_gettime(CLOCK_REALTIME, &tEnd);

  /* Print */
  execTime = (tEnd.tv_sec  - tStart.tv_sec) +
    1e-9*(tEnd.tv_nsec  - tStart.tv_nsec);
  printf("Time: %f   Number of iterations: %d\n", execTime, k);
  printf("Temperature of element T(1,1) = %.17f\n", T[1][0]);

  /* Save to file. */
  save_res("result_c.dat", T);

  return 0;
}

