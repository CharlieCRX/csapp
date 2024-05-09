#include <stdio.h>
int main(){
	int a = 0x1234ff;
	unsigned char lsb = a & 0xff;
	printf("lsb = %x = %d\n",lsb,lsb);

	lsb = ~lsb;
	printf("~lsb = %x = %d\n",lsb,lsb);

	printf("!~lsb = %d\n",!lsb);
	printf("~0xff = %x",~0xff);
	return 0;
}
