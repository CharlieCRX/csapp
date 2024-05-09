#include <stdio.h>
int main(){
	char a = 0xff;
	unsigned int b = 0xff;
	if(a == b){
		printf("char a = unsigned int b\n");
	}else{
		printf("they are not equal.\n");
	}
	printf((a == 0xff) ? "a == 0xff\n":"a != 0xff,a = %x\n",a);
	return 0;
}
