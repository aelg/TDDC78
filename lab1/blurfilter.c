/*
File: blurfilter.c

Implementation of blurfilter function.

*/
#include <stdio.h>
#include "blurfilter.h"
#include "ppmio.h"


inline pixel* pix(pixel* image, const int xx, const int yy, const int xsize)
{
  register int off = xsize*yy + xx;

#ifdef DBG
  if(off >= MAX_PIXELS) {
    fprintf(stderr, "\n Terribly wrong: %d %d %d\n",xx,yy,xsize);
  }
#endif
  return (image + off);
}

void *blurfilter(void *attr){
  /*Extract data from the struct.*/
  struct blurfilter_attr *_attr = (struct blurfilter_attr*) attr;
  int xsize = _attr->xsize, ysize = _attr->ysize, starty = _attr->starty, stopy = _attr->stopy, radius = _attr->radius;
  pixel *src = _attr->src, *dst = _attr->dst;
  double *w = _attr->w;

  int x,y, wi;
  double r,g,b,n, wc;
  pixel *pos = pix(src, 0, starty, xsize), *tpos;

  for (y = starty; y<stopy; ++y) {
    for (x=0; x<xsize; ++x) {
      r = w[0] * pos->r;
      g = w[0] * pos->g;
      b = w[0] * pos->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
        wc = w[wi];
        tpos = pos-wi;
        if(x-wi >= 0) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
        tpos = pos+wi;
        if(x+wi < xsize) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
      }
      tpos = pix(dst,y,x, ysize);
      tpos->r = r/n;
      tpos->g = g/n;
      tpos->b = b/n;
      ++pos;
    }
  }
  return 0;
}

