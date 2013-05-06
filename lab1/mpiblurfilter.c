/*
File: blurfilter.c

Implementation of blurfilter function.

*/
#include <stdio.h>
#include <stdlib.h>
#include "mpiblurfilter.h"
#include "ppmio.h"

void blurfilter(char* start, int xsize, int ysize, 
    int ystart, int yend, double *w, int radius){

  int x,y, wi;
  double r,g,b,n, wc;
  pixel *dst = malloc(xsize*(yend-ystart)*3);
  pixel *src_pos = (pixel*)start, *dst_pos = dst, *tpos;

  for (y = ystart; y<yend; ++y) {
    for (x=0; x<xsize; ++x) {
      r = w[0] * src_pos->r;
      g = w[0] * src_pos->g;
      b = w[0] * src_pos->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
        wc = w[wi];
        tpos = src_pos-wi*xsize;
        if(y-wi >= 0) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
        tpos = src_pos+wi*xsize;
        if(y+wi < ysize) {
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
  dst_pos = (pixel*)start;
  for (y = ystart; y<yend; ++y) {
    for (x=0; x<xsize; ++x) {
      r = w[0] * src_pos->r;
      g = w[0] * src_pos->g;
      b = w[0] * src_pos->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
        wc = w[wi];
        tpos = src_pos-wi;
        if(x-wi >= 0) {
          r += wc * tpos->r;
          g += wc * tpos->g;
          b += wc * tpos->b;
          n += wc;
        }
        tpos = src_pos+wi;
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

