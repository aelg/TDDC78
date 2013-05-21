#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <inttypes.h>
#include <VT.h>
#include "coordinate.h"
#include "definitions.h"
#include "physics.h"
#include "structures.h"

#define ITERATIONS 2000
#define TIME_STEP 1

#define LEFT 0
#define UP 1
#define DOWN 2
#define RIGHT 3

float min_x, max_x, min_y, max_y;

/* Checks if a particle is outside the region
 * and returns where it should be sent. */
int is_outside(particle_t *p){
  if(p->pcord.x < min_x) return LEFT;
  if(p->pcord.x >= max_x) return RIGHT;
  if(p->pcord.y < min_y) return DOWN;
  if(p->pcord.y >= max_y) return UP;
  return -1;
}

/* Calculates processor grid layout */
void calc_layout(int nprocs, int *rows, int *cols){
  int i;
  for(i = 1; i < sqrt((double)nprocs) + 0.01; ++i){
    if(nprocs % i == 0){
      *rows = i;
      *cols = nprocs/i;
    }
  }
}

double randf(){
  return ((double)rand()/(double)RAND_MAX);
}

int main (argc, argv)
  int argc;
  char *argv[];
{
  int neighbours[4];
  int dims[2];
  int rows, cols;
  int periods[2] = {1, 1};
  int reorder = 1;
  MPI_Comm grid_comm;
  MPI_Datatype pcord_mpi;
  double start_time = 0, end_time = 0, run_time;
  double vert_size, horiz_size, max_v;
  
  int rank = 0, nprocs = 1;
  int my_coords[2], coords[2];
  int i, j, k;
  double pressure, V, T, R, Ttot, ptot;
  int N;
  pcord_t send_buffs[4][COMM_BUFFER_SIZE];
  pcord_t recv_buffs[4][COMM_BUFFER_SIZE];
  int send_counts[4];
  int const dx[4] = {-1, 0, 0, 1};
  int const dy[4] = {0, 1, -1, 0};
  MPI_Request request[4];
  MPI_Status status[4];

  part_list_t particles;
  collision_list_t collisions;
  particle_t *p1, *p2;
  collision_pair_t *c1;

  cord_t box;

  int classhandle;
  int pressure_counter;
  int particle_counter;
  double pressure_bounds[2] = {0, 1e56};
  int64_t particle_bounds[2] = {0, 10000};
  int collision_handle;
  
  /* starts MPI */
  MPI_Init (&argc, &argv);  
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  /* Init pressure counter */
  VT_classdef("Simulate", &classhandle);
  VT_countdef("Pressure", classhandle, 
      VT_COUNT_FLOAT | VT_COUNT_ABSVAL | VT_COUNT_VALID_SAMPLE, VT_ME, 
      pressure_bounds, "p", &pressure_counter);
  VT_countdef("Particels", classhandle, 
      VT_COUNT_INTEGER64 | VT_COUNT_ABSVAL | VT_COUNT_VALID_SAMPLE, VT_ME, 
      particle_bounds, "p", &particle_counter);
  VT_funcdef("Collision analysis", classhandle, &collision_handle);

  /* Read arguments */
  if(argc != 5){
    printf("Bad # of arguments. Use %s N horiz_size\
 vert_size max_initial_velocity\n", argv[0]);
    MPI_Finalize();
    exit(1);
  }

  N = atoi(argv[1])/nprocs;
  horiz_size = atof(argv[2]);
  vert_size = atof(argv[3]);
  max_v = atof(argv[4]);

  
  /* Start timer */ 
  if(rank == 0) start_time = MPI_Wtime();

  /* Initialize */
  calc_layout(nprocs, &rows, &cols);
  if(rank == 0) printf("Layout: %dx%d\n", rows, cols);
  dims[1] = rows;
  dims[0] = cols;

  init_part_list(&particles);

  collisions.first = NULL;
  collisions.last = NULL;

  box.x0 = 0;
  box.x1 = horiz_size;
  box.y0 = 0;
  box.y1 = vert_size;

  pressure = 0;
  T = 0;
  V = horiz_size*vert_size;

  /* Create custom type */
  MPI_Type_contiguous(4, MPI_FLOAT, &pcord_mpi);
  MPI_Type_commit(&pcord_mpi);

  /* Init random generator 
   * (use rank to not seed with the same number for all ranks)*/
  srand((unsigned int) time(NULL) + rank*100);

  /* Create grid cart */
  MPI_Dims_create(nprocs, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, 
      dims, periods, reorder, &grid_comm);

  MPI_Cart_get(grid_comm, 2, dims, periods, my_coords);

  /* Get neighbours */
  for(i = 0; i<4; ++i){
    coords[0] = my_coords[0] + dx[i];
    coords[1] = my_coords[1] + dy[i];
    MPI_Cart_rank(grid_comm, coords, &(neighbours[i]));
  }

  /* Calculate local box */
  min_x = my_coords[0]*horiz_size/cols;
  if(my_coords[0] == cols-1)
    max_x = horiz_size;
  else
    max_x = (my_coords[0]+1)*horiz_size/cols;
  min_y = my_coords[1]*vert_size/rows;
  if(my_coords[1] == rows-1)
    max_y = vert_size;
  else
    max_y = (my_coords[1]+1)*vert_size/rows;

  /*printf("%f %f %f %f\n", min_x, max_x, min_y, max_y);*/


  /* Initiate particles. */
  for(i = 0; i < N; ++i){
    double angle = randf()*2*PI;
    double v = randf()*max_v;
    double x = randf()*(max_x - min_x) + min_x;
    double y = randf()*(max_y - min_y) + min_y;
    double vx = v * cos(angle);
    double vy = v * sin(angle);
    add_particle(&particles, make_particle(x, y, vx, vy));
    T += v*v/2;
  }
  T = T/(N);

  for(i = 0; i < ITERATIONS; ++i){

    /*if(rank == 0 && i%50 == 0) printf("%d\n", i);*/

    for(j = 0; j < 4; ++j){
      send_counts[j] = 0;
    }

    /* Look for collisions. Use first found */
    VT_enter(collision_handle, VT_NOSCL);
    p1 = particles.first;
    while(p1){
      p2 = p1->next;
      while(p2){
        double t = collide(&(p1->pcord), &(p2->pcord));
        if(t != -1){
          add_collision(&collisions, make_collision_pair(p1, p2, t));
          remove_particle(&particles, p1);
          remove_particle(&particles, p2);
          if(p1->next == p2) p1 = p2;
          break;
        }
        p2 = p2->next;
      }
      p1 = p1->next;
    }
    VT_leave(collision_handle);

    /* Move particles */
    p1 = particles.first;
    while(p1){
      int dir;

      feuler(&(p1->pcord), TIME_STEP);

      pressure += wall_collide(&(p1->pcord), box);

      /* Queue particle to be sent if outside region */
      if((dir = is_outside(p1)) != -1){
        particle_t *tmp;
        copy_pcord(&(send_buffs[dir][send_counts[dir]++]), &(p1->pcord));
        remove_particle(&particles, p1);
        tmp = p1;
        p1 = p1->next;
        free(tmp);
      }
      else{
        p1 = p1->next;
      }
    }
    /*printf("Second while\n");*/

    /* Interact */
    c1 = collisions.first;
    while(c1){
      int dir;
      interact(&(c1->p1->pcord), &(c1->p2->pcord), c1->t);

      /* Queue particle to be sent if outside region */
      if((dir = is_outside(c1->p1)) != -1){
        copy_pcord(&(send_buffs[dir][send_counts[dir]++]), &(c1->p1->pcord));
        free(c1->p1);
      }
      else{
        add_particle(&particles, c1->p1);
      }
      if((dir = is_outside(c1->p2)) != -1){
        copy_pcord(&(send_buffs[dir][send_counts[dir]++]), &(c1->p2->pcord));
        free(c1->p2);
      }
      else{
        add_particle(&particles, c1->p2);
      }

      c1 = c1->next;
    }
    empty_collision_list(&collisions);

    /* Send stuff */
    for(j = 0; j < 4; ++j){
      MPI_Isend(send_buffs[j], send_counts[j], pcord_mpi, 
          neighbours[j], 0, grid_comm, &(request[j]));
      /*printf("%d\n", send_counts[j]);*/
    }
    /* Receive stuff */
    for(j = 0; j < 4; ++j){
      MPI_Recv(recv_buffs[j], COMM_BUFFER_SIZE, pcord_mpi, 
          MPI_ANY_SOURCE, 0, grid_comm, &(status[j]));
    }
    MPI_Waitall(4, request, MPI_STATUS_IGNORE);
    for(j = 0; j < 4; ++j){
      int count;
      MPI_Get_count(&(status[j]), pcord_mpi, &count);
      for(k = 0; k < count; ++k){
        add_particle(&particles, make_particle(recv_buffs[j][k].x, 
              recv_buffs[j][k].y, recv_buffs[j][k].vx, recv_buffs[j][k].vy));
      }
    }
    VT_countval(1, &pressure_counter, &pressure);
    VT_countval(1, &particle_counter, &(particles.num_particles));
  }
  MPI_Reduce(&T, &Ttot, 1, MPI_DOUBLE, MPI_SUM, 0, grid_comm);
  MPI_Reduce(&pressure, &ptot, 1, MPI_DOUBLE, MPI_SUM, 0, grid_comm);

  /*printf("rank %d #particles: %d\n", rank, particles.num_particles);*/

  /* Free particles memory */
  p1 = particles.first;
  while(p1){
    p2 = p1->next;
    free(p1);
    p1 = p2;
  }

  /* Print result. */
  if(rank == 0){
    float wall_length = (vert_size+horiz_size)*2;
    end_time = MPI_Wtime();
    run_time = end_time - start_time;
    printf("Simulated %d timesteps on %d processors in %g secs\n\n", 
        i, nprocs, run_time);

    T = Ttot/nprocs;
    R = ptot*V/(N*nprocs*T*wall_length*ITERATIONS);
    printf("Pressure: %.2f\n", ptot/wall_length);
    printf("Temp: %.2f\n", T);
    printf("Volume: %.2e\n", V);
    printf("N: %d\n\n", N*nprocs);
    printf("R: %.2f\n", R);
  }


  MPI_Finalize();
  return 0;
}

