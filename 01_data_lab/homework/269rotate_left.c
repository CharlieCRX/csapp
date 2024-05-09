#include <stdio.h>
#include <assert.h>
/*
* Do rotating left shift. Assume 0 <= n < w
* Examples when x = 0x12345678 and w = 32:
* n=4 -> 0x23456781, n=20 -> 0x67812345
*/
unsigned rotate_left(unsigned x, int n){
	int w = sizeof(unsigned) << 3;
	unsigned left_shift_x = x << n;
	//printf("left = %x\n",left_shift_x);
	unsigned lower_bit_x = x >> (w - n -1) >> 1;
	//printf("lower = %x\n",lower_bit_x);
	return left_shift_x | lower_bit_x;
}
int main(){
	rotate_left(0x12345678,4);
	rotate_left(0x12345678,0);
	return 0;
}
