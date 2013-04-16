#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include "ppmio.h"
#include "mpithresfilter.h"


#define uint unsigned int 

int main (argc, argv)
int argc;
char *argv[];
{
	int *counts, *displs;
	int rank, nproc;
	int pos, i, blocksize ,totsize, xsize, ysize, colmax;
	pixel *src, *l_src, *l_dst;
	int *blockstart;
	const int pixel_length = 3;
	int recvcount;
	int64_t l_sum;
	int64_t g_sum;

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

		/* Calculate offsets for data to be sent to processes. */
		
		totsize = xsize*ysize*pixel_length;
		blocksize = totsize/(pixel_length*nproc); /* blocksize in pixels */
		blockstart = malloc(sizeof(int)*(nproc+1));
		counts = malloc(sizeof(int)*nproc);
		pos = 0;

		for(i = 0; i <= nproc; ++i){
			blockstart[i] = pos;
			if(i != nproc){
				pos += blocksize*pixel_length + (i < totsize%(pixel_length*nproc) ? 1:0);
				counts[i] = (pos-blockstart[i]);
			}
		}
		displs = malloc(sizeof(int)*nproc);
		memcpy(displs,counts,nproc); /* mpi_scatter demands that displs and counts not be read from the same space */
		MPI_Bcast(blockstart, nproc+1, MPI_INTEGER, 0, MPI_COMM_WORLD);

		/* blockstart[i] now contains starting line for data to be processed by rank i.
		 * blockstart[i+1] contains the line after the last line in data to be processed by rank i.*/
        /* counts[] is used by MPI_Gatherv and MPI_Scatterv to determine how much data each process processes */

	   
	}
	
	/* distribute the work */
	recvcount = blockstart[rank+1]-blockstart[rank];
	l_src = malloc(recvcount);
	
	MPI_Scatterv((char*) src, counts, displs, MPI_CHAR, l_src, recvcount,MPI_CHAR, 0, MPI_COMM_WORLD);

	/* calculate pixel sum */
	l_sum = thresfiltersum(l_src,recvcount/pixel_length);

	/* gather local sums, add them and redistribute total sum */
	MPI_Allreduce(&l_sum,&g_sum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	/* run filter */
	thresfilteravg(l_src,recvcount/pixel_length,g_sum);

	/* gather result */
	MPI_Gatherv(l_src,recvcount,MPI_CHAR,src,counts,displs,MPI_CHAR,0,MPI_COMM_WORLD);

	/* print output */
	if (rank == 0 && write_ppm(argv[3],xsize,ysize,(char *)src) != 0) 
		exit(1);

	MPI_Finalize();
	return 0;
}

