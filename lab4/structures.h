#include "definitions.h"

struct part_list{
  particle_t *first, *last;
};

struct collision_pair{
  particle_t *p1, *p2;
  struct collision_pair *next, *prev;
  double t;
};

struct collision_list{
  struct collision_pair *first, *last;
};

typedef struct part_list part_list_t;
typedef struct collision_pair collision_pair_t;
typedef struct collision_list collision_list_t;

particle_t* make_particle(double x, double y, double vx, double vy);
void add_particle(part_list_t *l, particle_t *p);
void remove_particle(part_list_t *l, particle_t *p);

collision_pair_t* make_collision_pair(particle_t *p1, particle_t *p2, double t);
void add_collision(collision_list_t *l, collision_pair_t *p);
void remove_collision(collision_list_t *l, collision_pair_t *p);
void empty_collision_list(collision_list_t *l);

