#include <string.h>
#include "structures.h"

/* Implements two linked list structures 
 * to store particles and collisions. */

void copy_pcord(pcord_t *to, pcord_t *from){
  memcpy(to, from, sizeof(pcord_t)/sizeof(char));
}

particle_t* make_particle(double x, double y, double vx, double vy){
  particle_t *tmp = malloc(sizeof(particle_t)/sizeof(char));
  tmp->pcord.x = x;
  tmp->pcord.y = y;
  tmp->pcord.vx = vx;
  tmp->pcord.vy = vy;
  tmp->prev = NULL;
  tmp->next = NULL;
  return tmp;
}

void init_part_list(part_list_t *p){
  p->first = NULL;
  p->last = NULL;
  p->num_particles = 0;
}

void add_particle(part_list_t *l, particle_t *p){
  if (l->last == NULL){
    l->first = p;
    l->last = p;
    p->next = NULL;
    p->prev = NULL;
  }
  else{
    l->last->next = p;
    p->prev = l->last;
    l->last = p;
    p->next = NULL;
  }
  ++l->num_particles;
}

void remove_particle(part_list_t *l, particle_t *p){
  --l->num_particles;
  if(l->first == p && l->last == p){
    l->first = NULL;
    l->last = NULL;
    return;
  }
  if(l->first == p){
    l->first = p->next;
    l->first->prev = NULL;
    return;
  }
  if(l->last == p){
    l->last = p->prev;
    l->last->next = NULL;
    return;
  }
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

collision_pair_t* make_collision_pair(particle_t *p1, 
    particle_t *p2, double t){
  collision_pair_t *tmp = malloc(sizeof(collision_pair_t)/sizeof(char));
  tmp->p1 = p1;
  tmp->p2 = p2;
  tmp->t = t;
  tmp->prev = NULL;
  tmp->next = NULL;
  return tmp;
}

void add_collision(collision_list_t *l, collision_pair_t *p){
  if (l->last == NULL){
    l->first = p;
    l->last = p;
    p->next = NULL;
    p->prev = NULL;
  }
  else{
    l->last->next = p;
    p->prev = l->last;
    l->last = p;
    p->next = NULL;
  }
}

void remove_collision(collision_list_t *l, collision_pair_t *p){
  if(l->first == p){
    l->first = p->next;
    l->first->prev = NULL;
    return;
  }
  if(l->last == p){
    l->last = p->prev;
    l->last->next = NULL;
    return;
  }
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

void empty_collision_list(collision_list_t *l){
  collision_pair_t *c = l->first, *tmp;
  while(c){
    tmp = c->next;
    free(c);
    c = tmp;
  }
  l->first = NULL;
  l->last = NULL;
}
