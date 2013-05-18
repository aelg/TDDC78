#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
/*#include <mpi.h>*/
#include <inttypes.h>
#include "coordinate.h"
#include "definitions.h"
#include "physics.h"
#include "structures.h"

#define ROWS 1
#define COLS 1

#define ITERATIONS 1000
#define TIME_STEP 1

float wall_x0[4] = {0,             0,              0,              BOX_HORIZ_SIZE};
float wall_x1[4] = {0,             BOX_HORIZ_SIZE, BOX_HORIZ_SIZE, BOX_HORIZ_SIZE};
float wall_y0[4] = {0,             0,              BOX_VERT_SIZE,  0};
float wall_y1[4] = {BOX_VERT_SIZE, 0,              BOX_VERT_SIZE,  BOX_VERT_SIZE};

double randf(){
  return ((double)rand()/(double)RAND_MAX);
}

int main (argc, argv)
  int argc;
  char *argv[];
{
  int dims[2] = {ROWS, COLS};
  int periods[2] = {1, 1};
  int reorder = 1;
  /*MPI_Comm grid_comm;*/
  
  int rank = 0, nprocs = 1;
  int my_coords[2], coords[2];
  int right, left, up, down;
  int i, j;
  int min_x, max_x, min_y, max_y;
  double pressure, V, T, R, N;

  part_list_t particles;
  collision_list_t collisions;
  particle_t *p1, *p2;
  collision_pair_t *c1;

  cord_t walls[4];
  
  //MPI_Init (&argc, &argv);  /* starts MPI */
  //MPI_Comm_rank (MPI_COMM_WORLD, &rank);  /* get current process id */
  //MPI_Comm_size (MPI_COMM_WORLD, &nprocs);  /* get number of processes */
  
  /* Initialize */
  particles.first = NULL;
  particles.last = NULL;

  collisions.first = NULL;
  collisions.last = NULL;

  for(i = 0; i < 4; ++i){
    walls[i].x0 = wall_x0[i];
    walls[i].x1 = wall_x1[i];
    walls[i].y0 = wall_y0[i];
    walls[i].y1 = wall_y1[i];
  }

  pressure = 0;
  T = 0;
  V = BOX_HORIZ_SIZE*BOX_VERT_SIZE;
  N = INIT_NO_PARTICLES;

  /* Init random generator (use rank to not seed with the same number for all rank)*/
  srand((unsigned int) time(NULL) + rank*100);

  /* Create grid cart */
  /*MPI_Dims_create(nprocs, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &grid_comm);

  MPI_Cart_get(grid_comm, 2, dims, periods, my_coords);*/

  /* Get neighbours */
  /*coords[0] = my_coords[0];
  coords[1] = my_coords[1] + 1;
  MPI_Cart_rank(grid_comm, coords, &right);
  coords[0] = my_coords[0] - 1;
  coords[1] = my_coords[1];
  MPI_Cart_rank(grid_comm, coords, &up);
  coords[0] = my_coords[0];
  coords[1] = my_coords[1] -1;
  MPI_Cart_rank(grid_comm, coords, &left);
  coords[0] = my_coords[0]+1;
  coords[1] = my_coords[1];
  MPI_Cart_rank(grid_comm, coords, &down);*/

  /* Calculate local box */
  min_x = my_coords[0]*BOX_HORIZ_SIZE/COLS;
  if(my_coords[0] == COLS-1)
    max_x = BOX_HORIZ_SIZE;
  else
    max_x = (my_coords[0]+1)*BOX_HORIZ_SIZE/COLS;
  min_y = my_coords[1]*BOX_VERT_SIZE/ROWS;
  if(my_coords[1] == ROWS-1)
    max_y = BOX_VERT_SIZE;
  else
    max_y = (my_coords[1]+1)*BOX_VERT_SIZE/ROWS;


  /* Initiate particles (TODO: this has uniform speed should probably be gaussian). */
  for(i = 0; i < INIT_NO_PARTICLES; ++i){
    double angle = randf()*PI;
    double v = randf()*MAX_INITIAL_VELOCITY;
    double x = randf()*(max_x - min_x) + min_x;
    double y = randf()*(max_y - min_y) + min_y;
    double vx = v * cos(angle);
    double vy = v * sin(angle);
    add_particle(&particles, make_particle(x, y, vx, vy));
    T += v*v;
  }
  T = T/INIT_NO_PARTICLES;

  for(i = 0; i < ITERATIONS; ++i){
    printf("%d\n", i);

    /* Look for collisions. Use first found */
    p1 = particles.first;
    while(p1){
      p2 = p1->next;
      while(p2){
        double t = collide(&(p1->pcord), &(p2->pcord));
        if(t != -1){
          add_collision(&collisions, make_collision_pair(p1, p2, t));
          remove_particle(&particles, p1);
          remove_particle(&particles, p2);
          break;
        }
        p2 = p2->next;
      }
      p1 = p1->next;
    }
    /*printf("First while\n");*/

    /* Move particles */
    p1 = particles.first;
    while(p1){

      feuler(&(p1->pcord), TIME_STEP);

      for(j = 0; j < 4; ++j){
        pressure += wall_collide(&(p1->pcord), walls[i]);
      }
      p1 = p1->next;
    }
    /*printf("Second while\n");*/

    /* Interact */
    c1 = collisions.first;
    while(c1){
      interact(&(c1->p1->pcord), &(c1->p2->pcord), c1->t);
      add_particle(&particles, c1->p1);
      add_particle(&particles, c1->p2);
      c1 = c1->next;
    }
    empty_collision_list(&collisions);
    /*printf("Third while\n");*/
  }

  R = pressure*V/(N*T);
  printf("Pressure: %f\n", pressure);
  printf("R: %f\n", R);


  /*MPI_Finalize();*/
  return 0;
}

