#include <stdio.h>
#include <assert.h>
typedef unsigned char * byte_pointer;
int is_little_endian(){
	int test_num = 0xff;
	byte_pointer start = (byte_pointer) &test_num;
	if(*start == 0xff){
		return 1;
	}
	return 0;
}

int main(){
	assert(is_little_endian());
	return 0;
}
