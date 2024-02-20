#include <stdio.h>
void replace_swap(int *a,int *b){
	*b = *a ^ *b;
	*a = *a ^ *b;
	*b = *b ^ *a;
}

int main(){
	int a = 12;
	int b = 2021;
	replace_swap(&a,&b);
	printf("swap:a = %d,b = %d.\n",a,b);
	return 0;
}
