#include "mpithresfilter.h"

#define uint unsigned int 

int64_t thresfiltersum(pixel *src,long nump){
	int64_t sum;
	int i;

	for(i = 0, sum = 0; i < nump; i++) {
		sum += (uint)src[i].r 
			+ (uint)src[i].g 
			+ (uint)src[i].b;
	}

	return sum;
}

void thresfilteravg(pixel *src,long nump,int64_t sum) {
	uint i,psum;

	for(i = 0, psum = 0; i < nump; i++) {
		psum += (uint)src[i].r 
			+ (uint)src[i].g 
			+ (uint)src[i].b;

		if(sum > psum) {
			src[i].r = src[i].g = src[i].b = 0;
		}
		else {
			src[i].r = src[i].g = src[i].b = 255;
		}
	}
	
	return 0;
}
