#include <stdio.h>
#include "structures.h"

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
}

void remove_particle(part_list_t *l, particle_t *p){
  if(l->first == p){
    printf("First removed\n");
    l->first = p->next;
    l->first->prev = NULL;
    return;
  }
  if(l->last == p){
    printf("Last removed\n");
    l->last = p->prev;
    l->last->next = NULL;
    return;
  }
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

collision_pair_t* make_collision_pair(particle_t *p1, particle_t *p2, double t){
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
  l->first = NULL;
  l->last = NULL;
}
