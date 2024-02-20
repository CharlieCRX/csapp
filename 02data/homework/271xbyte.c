#include <stdio.h>
#include <assert.h>
/* Declaration of data type where 4 bytes are packed
into an unsigned */
typedef unsigned packed_t;
/* Extract byte from word. Return as signed integer */
int xbyte(packed_t word, int bytenum){
	return (int) word << ((3 - bytenum) << 3) >> ((sizeof(int) - 1) << 3);
}
int main(){
	assert(xbyte(0x12345678, 0) == 0x78);
	assert(xbyte(0x12345678, 1) == 0x56);
	assert(xbyte(0x12345678, 2) == 0x34);
	assert(xbyte(0x12345678, 3) == 0x12);

	assert(xbyte(0x123488, 0) == 0xFFFFFF88);
	return 0;

}
