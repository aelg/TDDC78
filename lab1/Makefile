
CFLAGS = -Wall -Wextra -pedantic -O3

LFLAGS= -lpthread -lrt -lm

all: blurc thresc mpiblurc mpithreshold

clean:
	-$(RM) *.o blur thresc mpiblurc

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

mpithreshold.o: mpithreshold.c
	mpicc -o mpithreshold.o -c $(CFLAGS) mpithreshold.c

mpithreshold: mpithreshold.o ppmio.o
	mpicc -o $@ mpithreshold.o ppmio.o $(LFLAGS)

mpithresfilter.o: mpithresfilter.c mpithresfilter.h
	mpicc -o mpithresfilter.o -c $(CFLAGS) mpithresfilter.c

mpithres.o: mpithres.c
	mpicc -o mpithres.o -c $(CFLAGS) mpithres.c

mpithresc: mpithres.o ppmio.o mpithresfilter.o
	mpicc -o $@ mpithres.o ppmio.o thresfilter.o $(LFLAGS)
