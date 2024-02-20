#include <stdio.h>
#include <assert.h>
int odd_ones(unsigned x){
	x ^= x >> 16;
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	x &= 1;
	return x;
}

int main(){
	assert(odd_ones(0x11111));
	assert(!odd_ones(0x11));
	assert(!odd_ones(0xf028));
	return 0;
}
