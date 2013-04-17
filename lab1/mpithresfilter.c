#include "mpithresfilter.h"

#define uint unsigned int 

SUM_TYPE thresfiltersum(pixel *src,long nump){
	SUM_TYPE sum;
	int i;

	for(i = 0, sum = 0; i < nump; i++) {
		sum += (uint)src[i].r 
			+ (uint)src[i].g 
			+ (uint)src[i].b;
	}

	return sum;
}

void thresfilteravg(pixel *src,long nump,SUM_TYPE sum) {
	uint i,psum;
	printf("sum:%lf",sum);

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
		psum = 0;
	}
	
	return 0;
}
