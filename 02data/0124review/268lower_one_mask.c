#include <stdio.h>
#include <assert.h>
/*
* Mask with least signficant n bits set to 1
* Examples: n = 6 --> 0x3F, n = 17 --> 0x1FFFF
* Assume 1 <= n <= w
*/
int lower_one_mask(int n){
	int mask = (1 << (n - 1) << 1) - 1;
	return mask;
}
int main(){
	assert(lower_one_mask(6) == 0x3F);
	assert(lower_one_mask(17) == 0x1FFFF);
	assert(lower_one_mask(32) == 0xFFFFFFFF);
	assert(lower_one_mask(1) == 1);
	return 0;
}
