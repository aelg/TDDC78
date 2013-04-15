#include "thresfilter.h"

#define uint unsigned int 

void *thresfiltersum(void *attr){
	int64_t sum;
	int i;
	size_t nump;
	struct thresfilter_attr *_attr = (struct thresfilter_attr*) attr;

	nump = _attr->end - _attr->start;

	for(i = 0, sum = 0; i < nump; i++) {
		sum += (uint)_attr->start[i].r 
			+ (uint)_attr->start[i].g 
			+ (uint)_attr->start[i].b;
	}
	pthread_mutex_lock(_attr->lock);
	*(_attr->sum)+=sum;
	pthread_mutex_unlock(_attr->lock);

	return 0;
}

void *thresfilteravg(void *attr) {
	uint i,psum;
	struct thresfilter_attr *_attr = (struct thresfilter_attr*) attr;
	size_t nump;

	nump = _attr->end - _attr->start;

	for(i = 0, psum = 0; i < nump; i++) {
		psum = (uint)_attr->start[i].r 
			+ (uint)_attr->start[i].g 
			+ (uint)_attr->start[i].b;
		if(*(_attr->sum) > psum) {
			_attr->start[i].r = _attr->start[i].g = _attr->start[i].b = 0;
		}
		else {
			_attr->start[i].r = _attr->start[i].g = _attr->start[i].b = 255;
		}
	}
	
	return 0;
}
