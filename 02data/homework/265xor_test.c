#include <stdio.h>
void printBinary(unsigned x){
	//int size = (sizeof(unsigned) << 3) - 1;
	//printf("size = %d\n",size);
	for(int i = (sizeof(unsigned) << 3) - 1; i >= 0; i--){
		unsigned mask = 1u << i;
		putchar((mask & x) ?  '1' : '0');
		if(i % 4 == 0){
			putchar(' ');
		}
	}
	putchar('\n');
}
int main(){
	unsigned x = 0x12345678;
	printBinary(x);
	x ^= x >> 16;
	printBinary(x);
	
	x ^= x >> 8;
	printBinary(x);

	x ^= x >> 4;
	printBinary(x);
	
	x ^= x >>2;
	printBinary(x);

	x ^= x >> 1;
	printBinary(x);

	x &= 0x1;
	printBinary(x);
	return 0;
}
