#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>

#define SIZE 10002
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
  int  updatedLines[SIZE];
  double *ptr, *end_ptr;
  unsigned int error, glob_error;
  double execTime;
  double tStart, tEnd;
  int k, i, j;
  int rank, numThreads;
  int startline, endline;
  int glob_num_threads;

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

  glob_error = 1;
#pragma omp parallel shared(tol, T, k, glob_error) 
      private(i,j, rank, error, startline, endline) 
      reduction(min : numThreads)
  {
    /* Divide work between threads.*/
    rank = omp_get_thread_num();
    numThreads = omp_get_num_threads();
    if(rank == 0){
      printf("Running with %d threads.\n", numThreads);
      glob_num_threads = numThreads;
    }
    startline = (rank*n1)/numThreads;
    endline = ((rank+1)*n1)/numThreads;
    /* Some ugly stuff to set end end start correct.
     * The last region don't need to use a saved line for the last run.
     * Instead it runs the standard loop for the last line too, as this is never updated.*/
    if(rank == 0) ++startline;
    if(endline == n1) ++endline;
    for(k = 0; k < maxiter && glob_error;){
      int first = 1;
#pragma omp single
      {
        ++k;
      }
      error = 0;
      glob_error = 0;
      /* Save data that are needed later.*/
      memcpy(prevl[rank], T[startline-1], n*sizeof(double)/sizeof(char));
      memcpy(prevf[rank], T[startline], n*sizeof(double)/sizeof(char));
#pragma omp barrier
      for(i = startline; i < endline-1; ++i){
        for(j = 1; j < n1; ++j){
          double tmp = prevl[rank][j];
          prevl[rank][j] = (T[i-1][j] + T[i+1][j] + T[i][j+1] + T[i][j-1])/4.0;
          T[i-1][j] = tmp;

          /*Tried some different approaches for error calculation, 
		   *this seems to be the fastest.
           * Not sure if fabs() branches, but it doesn't seem to improve 
		   * compared to code that doesn't branch.*/
          error = error | fabs(T[i][j] - prevl[rank][j]) > tol;
          /*error = max(error, fabs(T[i][j] - prevl[rank][j]));*/
          /*diff = T[i][j] - prevl[rank][j];
            error = error || diff > tol || -diff > tol;*/
        }
      }
      /* Special code for the last line in the threads region. 
	   * Uses data saved by the thread that processed this line.
       * The last thread doesn't need this.*/
      if(rank+1<numThreads){
        for(j = 1; j < n1; ++j){
          double tmp = prevl[rank][j];
          prevl[rank][j] = (T[i-1][j] + prevf[rank+1][j] + T[i][j+1] + T[i][j-1])/4.0;
          T[i-1][j] = tmp;

          error = error | fabs(T[i][j] - prevl[rank][j]) > tol;
        }
        ++i;
      }
      /* Barrier here in case one thread is done before the next one started it 
	   * doesn't overwrite the last line.*/
#pragma omp barrier
      memcpy(T[i-1]+1, prevl[rank]+1, (n-2)*sizeof(double)/sizeof(char));
      /* Critical, barrier and flush to synchronize error cheking */
#pragma omp critical
      {
        glob_error = glob_error | error;
      }
#pragma omp barrier
#pragma omp flush(glob_error,k)
    }
  }

  /* Get time */
  tEnd = omp_get_wtime();

  /* Print */
  execTime = tEnd-tStart;
  printf("Outer loop parallel\n");
  printf("Time: %f   Number of iterations: %d\n", execTime, k+1);
  printf("Size: %d\n", n2);
  printf("MFLOPS: %f\n", ((float)n2)*n2*5*(k+1)/(execTime*1000000));
  printf("MFLOPS per thread: %f\n", 
		 ((float)n2)*n2*5*(k+1)/((execTime*glob_num_threads)*1000000));
  printf("Temperature of element T(1,1) = %.17f\n", T[1][1]);

  /* Save to file. */
  /*save_res("result_c.dat", T);*/

  return 0;
}

