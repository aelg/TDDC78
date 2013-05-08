CFLAGS = -Wall -Wextra -pedantic -O2

LFLAGS= -lrt -lm

all: laplsolv laplsolv_fortran laplsolv_openmp

clean:
	-$(RM) *.o laplsolv

laplsolv: laplsolv.c
	gcc -o laplsolv $(CFLAGS) $(LFLAGS) laplsolv.c

laplsolv_fortran: laplsolv.f90
	gfortran -o laplsolv_fortran $(CFLAGS) $(LFLAGS) laplsolv.f90

laplsolv_openmp: laplsolv_openmp.c
	gcc -o laplsolv_openmp -fopenmp $(CFLAGS) $(LFLAGS) laplsolv_openmp.c

