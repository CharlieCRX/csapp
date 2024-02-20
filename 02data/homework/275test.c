#include <stdio.h>
#include <assert.h>
int test1(unsigned x){
	int sig_x = x >> 31;
	return sig_x;
}
int main(){
	printf("%x\n",test1(0xFFFFFFFF));
	printf("%x\n",test1(0x12345678));
	printf("%x\n",test1(0x7FFFFFFF));
	return 0;
}
