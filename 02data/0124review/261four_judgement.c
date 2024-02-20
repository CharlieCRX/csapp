#include <stdio.h>
#include <assert.h>
int fun_all_one(int x){
	return !(x ^ ((int) -1));
}

int fun_all_zero(int x){
	return !x;
}

int fun_ls_byte_all_one(int x){
	int lb_x = x & 0xff;
	return !(lb_x ^ 0xff);
}

int fun_ms_byte_all_zero(int x){
	int arithm_shift_x = x >> ((sizeof(int) - 1) << 3);
	return fun_all_zero(arithm_shift_x);
}

int main(){
	int a = 0xffffffff;
	int b = 0;
	int c = 0x123456ff;
	int d = 0x123456;
	assert(fun_all_one(a));
	assert(fun_all_zero(b));
	assert(fun_ls_byte_all_one(c));
	assert(fun_ms_byte_all_zero(d));
	return 0;
}
