#include <stdio.h>
#include <assert.h>
typedef unsigned char* byte_pointer;
unsigned replace_byte1 (unsigned x, int index, unsigned char b){
	int sizeOfUnsigned = sizeof(unsigned);
	printf("size = %d\n",sizeOfUnsigned);
	assert((index <= sizeOfUnsigned));
	byte_pointer byte_start = (byte_pointer) &x;
	byte_start[index] = b;
	return x;
}

unsigned replace_byte2 (unsigned x, int index, unsigned char b){
	//ensure the legality of the boundary
	assert((index >= 0) && (index < sizeof(unsigned)));

	unsigned mask = (unsigned) 0xff << (sizeof(unsigned) << index);
	unsigned byte_rep = (unsigned) b << (sizeof(unsigned) << index);
	return (x & ~mask) | byte_rep;
}
int main(){
	unsigned x = replace_byte2(0x12345678, 2, 0xef);
	printf("x = %x\n",x);
	return 0;
}
