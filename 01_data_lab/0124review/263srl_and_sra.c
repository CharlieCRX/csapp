#include <stdio.h>
#include <assert.h>
unsigned srl(unsigned x, int k) {
	/* Perform shift arithmetically */
	unsigned xsra = (int) x >> k;
	/* create a (w -k)-bit mask to reamin the lower (w - k)-bit of xsra */
	int w = sizeof(int) << 3;
	int mask = -1 << (w - k);

	return xsra & ~mask;
}
int sra(int x, int k) {
	/* Perform shift logically */
	int xsrl = (unsigned) x >> k;

	/* Get the sign bit */
	int w = sizeof(int) << 3;
	int sign_data = xsrl & (1 <<  (w - k - 1));//equals: x & (1 << (w - 1))
	int sign_bit = !!sign_data;

	/* Create a mask that remains the first w bits */
	int mask = -1 << (w - k);
	mask &= (!sign_bit - 1);
	return xsrl | mask;
}

int main(){
	assert(srl(0x90123456, 8) == 0x901234);
	//assert(sra(0x12345678, 8) == 0x123456);
	assert(sra(0xff123456, 8) == 0xffff1234);
	return 0;
}
