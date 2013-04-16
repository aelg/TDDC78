#ifndef _THRESFILTER_H_
#define _THRESFILTER_H_

#include <inttypes.h>

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

int64_t thresfiltersum(pixel *src,long nump);
void thresfilteravg(pixel *src,long nump,int64_t sum);

#endif
