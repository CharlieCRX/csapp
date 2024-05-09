#include <stdio.h>
#include <assert.h>
int any_odd_one(unsigned x){
	unsigned mask = 0xAAAAAAAA;
	return !!(x & mask);
}
int main(){
	assert(!any_odd_one(0x55555555));
	assert(any_odd_one(0xa));
	return 0;
}
