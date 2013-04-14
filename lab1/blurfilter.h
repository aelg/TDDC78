/*
  File: blurfilter.h

  Declaration of pixel structure and blurfilter function.
    
 */

#ifndef _BLURFILTER_H_
#define _BLURFILTER_H_

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

struct blurfilter_attr{
  int xsize, ysize, radius, starty, stopy;
  double *w;
  pixel *src, *dst;
};

void *blurfilter(void *attr);

#endif
