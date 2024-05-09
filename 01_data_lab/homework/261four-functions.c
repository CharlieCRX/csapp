#include <stdio.h>
#include <assert.h>
int fun_A(int x){
	return !~x;
}

int fun_B(int x){
	return !x;
}

int fun_C(int x){
	int mask = 0xff;
	int lsb = x & mask;
	return !(lsb ^ mask);
}

int fun_C2(int x){
	return !~(x | ~0xff);
}

int fun_D(int x){
	x = x >> ((sizeof(int) - 1) << 3);
	return fun_B(x);
}

int main(){
	assert(fun_A(0xffffffff));
	assert(!fun_A(0x12345678));

	assert(fun_B(0));
	assert(!fun_B(0x1234));

	assert(fun_C2(0x1234ff));
	assert(!fun_C2(0x121212));

	assert(fun_D(0x1234));
	assert(!fun_D(0x11223344));
	return 0;
}
