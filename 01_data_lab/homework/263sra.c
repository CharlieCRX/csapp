#include <stdio.h>
int sra(int x, int k){
	int xsrl = (unsigned) x >> k;
	printf("xsrl = 0x%.8X\n",xsrl);
	int w = sizeof(int) << 3;
	int mask = -1 << (w - k);
	printf("mask = %X\n",mask);

	int mask_for_sign = 1 << (w -k -1);
	printf("mask_for_sign = %x\n",mask_for_sign);
	int topBitOnly = xsrl & (1 << (w- k - 1));
	printf("topBitOnly = %X\n",topBitOnly);
	int is_mask_flag = !topBitOnly -1;
	printf("is_mask_flag = %X\n",is_mask_flag);

	return xsrl | (is_mask_flag & mask);
}
int main(){
	int x = 0xefffabcd;
	printf("sra(0x%X,8) = 0x%.8X\n", x, sra(x, 8));
	return 0;
}

