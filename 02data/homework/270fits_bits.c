#include <stdio.h>
#include <assert.h>
/*
* Return 1 when x can be represented as an n-bit, 2’s-complement
* number; 0 otherwise
* Assume 1 <= n <= w
*/
int fits_bits(int x, int n){
	int w = sizeof(int) << 3;
	return x == (x << (w - n) >> (w - n));
}
int main(){
	assert(fits_bits(0x1234, 16));
	assert(!fits_bits(0x90, 8));
	assert(fits_bits(0x90, 9));
	assert(!fits_bits(0x7F, 7));
	assert(fits_bits(0x7F, 8));
	return 0;
}
