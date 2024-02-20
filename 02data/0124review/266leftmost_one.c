#include <stdio.h>
#include <assert.h>
/*
* Generate mask indicating leftmost 1 in x. Assume w=32.
* For example, 0xFF00 -> 0x8000, and 0x6600 --> 0x4000.
* If x = 0, then return 0.
*/
int leftmost_one(unsigned x){
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return (x >> 1) + (x & 1);
}
int main(){
	assert(leftmost_one(0xAFF) == 0x800);
	assert(leftmost_one(0) == 0);
	assert(leftmost_one(0xFFFFFFFF) == 0x80000000);
	return 0;
}
