#include <stdio.h>
#include <assert.h>
/*
* Mask with least signficant n bits set to 1
* Examples: n = 6 --> 0x3F, n = 17 --> 0x1FFFF
* Assume 1 <= n <= w
*/
int lower_one_mask(int n){
	int mask = 1 << (n - 1);
	mask = (mask << 1) - 1;
	return mask;
}

int main(){
	assert(lower_one_mask(7) == 0x7f);
	assert(lower_one_mask(32) == 0xffffffff);
	return 0;
}
