
CFLAGS = -Wall -Wextra -pedantic -O2

LFLAGS= -lpthread -lrt -lm

all: blurc thresc

clean:
	-$(RM) *.o blur thresc

ppmio.o: ppmio.c ppmio.h
	gcc -o ppmio.o -c $(CFLAGS) ppmio.c

gaussw.o: gaussw.c gaussw.h
	gcc -o gaussw.o -c $(CFLAGS) gaussw.c

blurfilter.o: blurfilter.c blurfilter.h
	gcc -o blurfilter.o -c $(CFLAGS) blurfilter.c

blurmain.o: blurmain.c
	gcc -o blurmain.o -c $(CFLAGS) blurmain.c

blurc: ppmio.o gaussw.o blurfilter.o blurmain.o
	gcc -o $@ ppmio.o gaussw.o blurfilter.o blurmain.o $(LFLAGS)

thresfilter.o: thresfilter.c thresfilter.h
	gcc -o thresfilter.o -c $(CFLAGS) thresfilter.c

thresmain.o: thresmain.c
	gcc -o thresmain.o -c $(CFLAGS) thresmain.c

thresc: thresmain.o ppmio.o thresfilter.o
	gcc -o $@ thresmain.o ppmio.o thresfilter.o $(LFLAGS)

mpippmio.o: ppmio.c ppmio.h
	mpicc -o mpippmio.o -c $(CFLAGS) ppmio.c

mpigaussw.o: gaussw.c gaussw.h
	mpicc -o mpigaussw.o -c $(CFLAGS) gaussw.c

mpiblurfilter.o: mpiblurfilter.c mpiblurfilter.h
	mpicc -o mpiblurfilter.o -c $(CFLAGS) mpiblurfilter.c

mpiblur.o: mpiblur.c
	mpicc -o mpiblur.o -c $(CFLAGS) mpiblur.c

mpiblurc: mpippmio.o mpigaussw.o mpiblurfilter.o mpiblur.o
	mpicc -o $@ mpippmio.o mpigaussw.o mpiblurfilter.o mpiblur.o $(LFLAGS)
