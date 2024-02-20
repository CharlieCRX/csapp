#include <stdio.h>
int main(){
	long a = 0x123456789A;
	unsigned char msk = 0xff;
	a = a | ~msk;
	printf("%x\n",a);
	unsigned char *byte_pointer = (char*)&a;
	for(int i = 0;i < sizeof(long); i++){
		printf(" %x",*(byte_pointer + i));
	}
	printf("\n");
	return 0;
}
