/**
	is-little-endian.c 20:12 14-Dec-23
*/
#include <stdio.h>
#include <assert.h>

typedef unsigned char * byte_pointer;
int is_little_endian(){
	int test_num = 0xff;
	byte_pointer byte_start = (byte_pointer) &test_num;
	if(byte_start[0] == 0xff){
		return 1;
	}
	return 0;
}

void is_little_endian_wrong(){
	int test_num = 0xff;
	char * byte_start = (char *)&test_num;
	printf("test_num = %d = 0x%x\n",test_num, test_num);
	/**
	**/
	printf("byte_start[0] = %d = 0x%x\n",byte_start[0], byte_start[0]);//will print "0xffffffff"?but why?
}

int main(){
	assert(is_little_endian());
	is_little_endian_wrong();
	return 0;
}

