#ifndef _THRESFILTER_H_
#define _THRESFILTER_H_

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

typedef double SUM_TYPE;

SUM_TYPE thresfiltersum(pixel *src,long nump);
void thresfilteravg(pixel *src,long nump,SUM_TYPE sum);

#endif
