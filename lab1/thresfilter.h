#ifndef _THRESFILTER_H_
#define _THRESFILTER_H_

#include <pthread.h>
#include <inttypes.h>

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

struct thresfilter_attr{
	pixel *start, *end;
	int64_t *sum;
	pthread_mutex_t *lock;
};

void *thresfiltersum(void *attr);
void *thresfilteravg(void *attr);

#endif
