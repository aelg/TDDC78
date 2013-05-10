#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>

#define SIZE 4002
#define MAX_THREADS 16

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
int const n1 = SIZE-1;
int const n2 = SIZE-2;
int const maxiter = 1000;
int main(){
  double tol = 1.0e-3;
  double (*T)[SIZE] = malloc(N*sizeof(double)/sizeof(char));
  double (*prevf)[SIZE] = malloc(n*MAX_THREADS*sizeof(double)/sizeof(char));
  double (*prevl)[SIZE] = malloc(n*MAX_THREADS*sizeof(double)/sizeof(char));
  int  updatedLines[SIZE], breaklines[MAX_THREADS];
  double *ptr, *end_ptr;
  unsigned int error;
  double execTime;
  double tStart, tEnd;
  int k, i, j, firstLine;
  int rank, numThreads;

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

  /* Get time */
  tStart = omp_get_wtime();

#pragma omp parallel shared(error, tol, T, k) private(i,j, rank, numThreads) default(shared)
  {
    /* Run calculation */
    rank = omp_get_thread_num();
    numThreads = omp_get_num_threads();
    if(rank == 0) printf("Running with %d threads.\n", numThreads);
    error = 1;
    for(k = 0; k < maxiter && error;){
#pragma omp single
        ++k;
      error = 0;
      firstLine = 1;
#pragma omp for schedule(static) private(j)
      for(j = 0; j < n; ++j){
        /*printf("rank: %d j: %d\n", rank, j);*/
        /*prev[j] = T[0][j];*/
        updatedLines[j] = 0;
      }
#pragma omp for firstprivate(firstLine) private(i,j) schedule(static) reduction(|| : error)
      for(i = 1; i < n1; ++i){
        int uu;
        breaklines[rank] = i;
        if(firstLine){
          firstLine = 0;
          updatedLines[i] = rank+1;
          memcpy(prevl[rank], T[i-1], n*sizeof(double)/sizeof(char));
          memcpy(prevf[rank], T[i], n*sizeof(double)/sizeof(char));
        }
        uu = updatedLines[i+1];

        /*Ordering of if and for is important to prevent branching inside the loop.
         * Two for loops are better than one.*/
        if(uu){
          --uu;
          for(j = 1; j < n1; ++j){
            double tmp = prevl[rank][j];
            prevl[rank][j] = (T[i-1][j] + prevf[uu][j] + T[i][j+1] + T[i][j-1])/4.0;
            T[i-1][j] = tmp;

            /*Tried some different approaches for error calculation, this seems to be the fastest.*/
            error = error || fabs(T[i][j] - prevl[rank][j]) > tol;
            /*error = max(error, fabs(T[i][j] - prevl[rank][j]));*/
            /*diff = T[i][j] - prevl[rank][j];
            error = error || diff > tol || -diff > tol;*/
          }
        }
        else{
          for(j = 1; j < n1; ++j){
            double tmp = prevl[rank][j];
            prevl[rank][j] = (T[i-1][j] + T[i+1][j] + T[i][j+1] + T[i][j-1])/4.0;
            T[i-1][j] = tmp;

            error = error || fabs(T[i][j] - prevl[rank][j]) > tol;
            /*if(error < fabs(T[i][j] - prevl[rank][j])) printf("%d %d %f %f\n", i, j, T[i][j], prevl[rank][j]);*/
            /*error = max(error, fabs(T[i][j] - prevl[rank][j]));*/
            /*diff = T[i][j] - prevl[rank][j];
            error = error || diff > tol || -diff > tol;*/
          }
        }
      }
      /*printf("error %f\n", error);*/
      for(j = 1; j < n1; ++j){
        T[breaklines[rank]][j] = prevl[rank][j];
      }
/*#pragma omp flush*/
    }
    /*fprintf(stderr, "error: %d tol: %f k: %d maxiter: %d rank: %d\n", error, tol, k, maxiter, rank);*/
  }

  /* Get time */
  tEnd = omp_get_wtime();

  /* Print */
  execTime = tEnd-tStart;
  printf("Time: %f   Number of iterations: %d\n", execTime, k+1);
  printf("Temperature of element T(1,1) = %.17f\n", T[1][1]);

  /* Save to file. */
  save_res("result_c.dat", T);

  return 0;
}

