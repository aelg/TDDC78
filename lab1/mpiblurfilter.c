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

void blurfilter(pixel* start, int xsize, int ystart, int yend, double *w, int radius){

  int x,y, wi;
  double r,g,b,n, wc;
  pixel *dst = malloc(xsize*(yend-ystart)*3);
  pixel *src_pos = start, *dst_pos = dst, *tpos;

  for (y = ystart; y<yend; ++y) {
    for (x=0; x<xsize; ++x) {
      r = w[0] * pos->r;
      g = w[0] * pos->g;
      b = w[0] * pos->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
        wc = w[wi];
        tpos = pos-wi*xsize;
        if(y-wi >= 0) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
        tpos = pos+wi*xsize;
        if(y+wi < yend) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
      }
      dst_pos->r = r/n;
      dst_pos->g = g/n;
      dst_pos->b = b/n;
      ++src_pos;
      ++dst_pos;
    }
  }
  src_pos = dst;
  dst_pos = start;
  for (y = ystart; y<yend; ++y) {
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
      dst_pos->r = r/n;
      dst_pos->g = g/n;
      dst_pos->b = b/n;
      ++src_pos;
      ++dst_pos;
    }
  }
  return;
}

