#include <stdio.h>
void show_cast1(){
	int a = 2147483647;
	unsigned b = 2147483648;
	printf("early:a = %d,b= %u.\n",a,b);
	int c = (int) b;
	printf("after:a = %d,b = %u, c = %d\n",a,b,c);
	printf("a > c = %d\n\n\n",a>c);
}
int main(){
	int a = 12345;
	printf("the size of unsigned is %ld\n",sizeof(unsigned));
	show_cast1();
	return 0;
	}
