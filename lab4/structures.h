#include "definitions.h"

struct part_list;
struct collision_pair;
struct collision_list;

typedef part_list part_list_t;
typedef collision_pair collision_pair_t;
typedef collision_list collision_list_t;

particle* make_particle(double x, double y, double vx, double vy);
void add_particle(part_list *l, particle *p);
void remove_particle(part_list *l, particle *p);

collision_pair* make_collision(particle *p1, particle *p2, double t);
void add_collision(collision_list *l, collision_pair *p);
void remove_collision(collision_list *l, collision *p);

