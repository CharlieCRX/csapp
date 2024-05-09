#include <stdio.h>
#include <assert.h>
unsigned replace_byte (unsigned x, int i, unsigned char b){
	//without loop
	if(i >= 4 || i < 0) assert(1);
	unsigned mask = ((unsigned) 0xff) << (i << 3);
	unsigned pos_byte = ((unsigned) b) << (i << 3);
	return (x & ~mask) | pos_byte;
}

int main(){
	unsigned rep_0 = replace_byte(0x12345678, 0, 0xAB);
	unsigned rep_3 = replace_byte(0x12345678, 3, 0xAB);

	assert(rep_0 == 0x123456AB);
	assert(rep_3 == 0xAB345678);
	return 0;
}	
