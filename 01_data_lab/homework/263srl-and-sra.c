#include <stdio.h>
#include <assert.h>
unsigned srl(unsigned x,int k){
	unsigned xsra = (int) x >> k;
	int mask = ~(-1 << (8*sizeof(unsigned) - k));
	return xsra & mask;
}
int sra(int x, int k){
	int xsrl = (unsigned) x >> k;
	int w = 8*sizeof(int);
	int sign_mask = 1 << (w - 1);
	int most_significant_bit = !(sign_mask & x);
	printf("most_significant_bit = %d\n",most_significant_bit);
	int final_mask = (-1 << (8*sizeof(int) - k)) & (most_significant_bit -1);
	printf("final_mask = %x\n",final_mask);
	return xsrl | final_mask;
}
int sra_from_answer(int x,int k){
	int xsrl = (unsigned) x >> k;
	int w = 8*sizeof(int);
	int mask = -1 << (w - k);
	int m = 1 << (w -1);
	mask = mask & (!(x&m) - 1);
	return xsrl | mask;
}
int main(){
	unsigned x = 0xff123456;
	printf("x = %x,srl(x) = %x\n",x,srl(x,5));
	int test_int = 0x12345678;
	assert(sra_from_answer(test_int,4) == test_int >> 4);
	return 0;
}
