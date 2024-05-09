#include <stdio.h>

int get_lowest_byte_of_int(int x){
	return x & 0xff;
}
int get_other_bytes_except_lowest_byte_of_int(int y){
	int mask = (-1) << 8;
	return mask & y;
}
int merge_partial_bytes1(int x,int y){
	//1.get the least significant byte of x
	x = get_lowest_byte_of_int(x);

	//2.get the other bytes except the lowest byte of y
	y = get_other_bytes_except_lowest_byte_of_int(y);
	
	//3.combine two vales to new value
	return x + y;
}
int main(){
	size_t x = 0x89ABCDEF;
	size_t y = 0x76543210;
	size_t mask = 0xff;
	size_t mergeValue = (x & mask) | (y & ~(mask));
	

	//int mergeValue = merge_partial_bytes1(x,y);
	printf("mergeValue = 0x%lx\n",mergeValue);
	return 0;
}
