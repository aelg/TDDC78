/*
File: blurfilter.c

Implementation of blurfilter function.

*/
#include <stdio.h>
#include "blurfilter.h"
#include "ppmio.h"


pixel* pix(pixel* image, const int xx, const int yy, const int xsize)
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
  // Extract data from the struct.
  struct blurfilter_attr *_attr = (struct blurfilter_attr*) attr;
  int xsize = _attr->xsize, ysize = _attr->ysize, starty = _attr->starty, stopy = _attr->stopy, radius = _attr->radius;
  pixel *src = _attr->src, *dst = _attr->dst;
  double *w = _attr->w;

  int x,y,x2,y2, wi;
  double r,g,b,n, wc;

  for (y=starty; y<stopy; ++y) {
    for (x=0; x<xsize; ++x) {
      r = w[0] * pix(src, x, y, xsize)->r;
      g = w[0] * pix(src, x, y, xsize)->g;
      b = w[0] * pix(src, x, y, xsize)->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
        wc = w[wi];
        x2 = x - wi;
        if(x2 >= 0) {
          r += wc * pix(src, x2, y, xsize)->r;
          g += wc * pix(src, x2, y, xsize)->g;
          b += wc * pix(src, x2, y, xsize)->b;
          n += wc;
        }
        x2 = x + wi;
        if(x2 < xsize) {
          r += wc * pix(src, x2, y, xsize)->r;
          g += wc * pix(src, x2, y, xsize)->g;
          b += wc * pix(src, x2, y, xsize)->b;
          n += wc;
        }
      }
      pix(dst,y,x, ysize)->r = r/n;
      pix(dst,y,x, ysize)->g = g/n;
      pix(dst,y,x, ysize)->b = b/n;
    }
  }
}



