#include <stdio.h>
int bitXor(int x, int y) {
	return ~(x&y)&~(~x&~y);
}

int main(){
	int a = 3 ^ 3;
	int b = 4 ^ 5;
	printf("a = %d,\nb = %d\n",a,b);
	printf("%x\n", bitXor(0x80000000, 0x80000000));
	return 0;
}
