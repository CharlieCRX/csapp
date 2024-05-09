#include <stdio.h>
#include <assert.h>
/*
* Do rotating left shift. Assume 0 <= n < w
* Examples when x = 0x12345678 and w = 32:
* n=4 -> 0x23456781, n=20 -> 0x67812345
*/
unsigned rotate_left(unsigned x, int n){
	int w = sizeof(unsigned) << 3;
	unsigned high_bit_data = x << n;
	//printf("high_bit_data = %x\n",high_bit_data);

	unsigned low_bit_data = x >> (w - n - 1) >> 1;
	//printf("low_bit_data = %x\n",low_bit_data);

	return high_bit_data | low_bit_data;
}

int main(){
	rotate_left(0x12345678, 4);
	rotate_left(0x12345678, 0);
	return 0;
}
