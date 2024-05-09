#include <stdio.h>
#include <assert.h>
int sra(int x, int k) {
	/* Perform shift logically */
	int xsrl = (unsigned) x >> k;

	int w = sizeof(int) << 3;
	/* Get the sign bit */
	int sign_data = x & (1 << (w - 1));
	int sign_bit = !!sign_data;

	int mask = (-1 << (w - k)) & (!sign_bit - 1);
	return xsrl | mask;
}

int main(){
	assert(sra(0xff123456, 8) == 0xffff1234);
	assert(sra(0x12345678, 8) == 0x123456);
	return 0;
}

