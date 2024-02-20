#include <stdio.h>
#include <assert.h>
int test_A(int a){
	return !~a;
}

int test_B(int b){
	return !b;
}

int test_C(int c){
	unsigned char leastSignificantByte =(unsigned char) (c & 0xff);
	int result = leastSignificantByte ^ 0xff;
	return !result;
}
//more simple but difficult to understand--12.17
int test_C2(int c){
	return !~(c | ~0xff);
}

int test_D(int d){
	int shift_val = (sizeof(int) - 1)<<3;
	int dright = d >> shift_val;
	int msb_of_d = dright & 0xff;
	return !msb_of_d;
}

int main(){
	int a = -1;
	int b = 0;
	int c = 0x1234ff;
	int d = 0x1234;
	assert(test_A(a));
	assert(test_B(b));
	assert(test_C2(c));
	assert(test_D(d));
	return 0;
}
