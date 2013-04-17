#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include "ppmio.h"
#include "mpithresfilter.h"


int main (argc, argv)
int argc;
char *argv[];
{
	int *counts, *displs;
	int rank, nproc;
	int pos, i, blocksize ,totsize, xsize, ysize, colmax;
	pixel *src, *l_src;
	int *blockstart;
	const int pixel_length = 3;
	int recvcount;
	SUM_TYPE l_sum;
	SUM_TYPE g_sum;

	MPI_Init (&argc, &argv);  /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &nproc);  /* get number of processes */

	/* Rank 0 reads file.*/
	if(rank == 0){
		src = malloc(sizeof(pixel)*MAX_PIXELS);
		if(!src){
			printf("Could not allocate memory, exiting");
			exit(1);
		}

		/* Take care of the arguments */

		if (argc != 4) {
			fprintf(stderr, "Usage: %s threads radius infile outfile\n", argv[0]);
			exit(1);
		}

		/* read file */
		if(read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
			exit(1);

		if (colmax > 255) {
			fprintf(stderr, "Too large maximum color-component value\n");
			exit(1);
		}
	}

	MPI_Bcast(&xsize, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&ysize, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

/* Calculate offsets for data to be sent to processes. */
		
		totsize = xsize*ysize; /* totsize in pixels */
		blocksize = totsize/(nproc); /* blocksize in pixels */
		blockstart = malloc(sizeof(int)*(nproc+1));
		counts = malloc(sizeof(int)*nproc);
		displs = malloc(sizeof(int)*nproc);
		pos = 0;
		displs[0] = 0;
		for(i = 0; i <= nproc; ++i){
			blockstart[i] = pos;
			if(i != nproc){
				pos += blocksize + (i < totsize%(nproc) ? 1:0);
				counts[i] = (pos-blockstart[i])*pixel_length;
				if(i > 0) displs[i] = displs[i-1] + counts[i];
			}
		}

	/* distribute the work */
	recvcount = (blockstart[rank+1]-blockstart[rank])*pixel_length;
	l_src = malloc(sizeof(char)*counts[rank]);
	MPI_Scatterv((char*) src, counts, displs, MPI_CHAR,(char *) l_src, counts[rank],MPI_CHAR, 0, MPI_COMM_WORLD);

	/* calculate pixel sum */
	l_sum = thresfiltersum(l_src,counts[rank]/pixel_length);

	/* gather local sums, add them and redistribute total sum */
	MPI_Allreduce(&l_sum,&g_sum,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

	/* run filter */
	thresfilteravg(l_src,counts[rank]/pixel_length,g_sum/totsize);

	/* gather result */
	MPI_Gatherv((char *)l_src,counts[rank],MPI_CHAR,(char *)src,counts,displs,MPI_CHAR,0,MPI_COMM_WORLD);

	/* print output */
	if (rank == 0 && write_ppm(argv[3],xsize,ysize,(char *)src) != 0) 
		exit(1);

	MPI_Finalize();
	return 0;
}

