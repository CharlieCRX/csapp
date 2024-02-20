#include <stdio.h>
int main(){
	int a = 65534;//bit-level = 1111 1111 1111 1110
	printf("a = %d = %b\n",a,a);
	double b = a;//transfer int value to double type value
	printf("b = %lf = %b = %x\n",b,b,b);
}
