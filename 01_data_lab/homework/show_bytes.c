#include <stdio.h>

typedef unsigned char * byte_pointer;
void test_show_bytes(int val);
void simple_show_a();
void simple_show_b();
void show_bytes(byte_pointer start,size_t len){
	size_t i;
	for(i = 0;i < len;i++){
		printf(" %.2x",start[i]);
	}
	printf("\n");
}
void show_int(int x){
	show_bytes((byte_pointer) &x,sizeof(int));
}

void show_float(float x){
	show_bytes((byte_pointer) &x,sizeof(float));
}

void show_pointer(void *x){
	show_bytes((byte_pointer) &x,sizeof(void *));
}

void test_show_bytes(int val){
	int ival = val;
	byte_pointer pval = (byte_pointer) &ival;
	show_int(ival);
	show_float((float) ival);
	show_pointer(pval);
}
void simple_show_a(){
	int a = 0x87654321;
	byte_pointer ptr = (byte_pointer) &a;
	show_bytes(ptr,1u);
	show_bytes(ptr,2u);
	show_bytes(ptr,3u);
}
void simple_show_b() {
	/* $begin simple-show-b */
	int val = 0x12345678;
	byte_pointer valp = (byte_pointer) &val;
	show_bytes(valp, 1); /* A. */
	show_bytes(valp, 2); /* B. */
	show_bytes(valp, 3); /* C. */
	/* $end simple-show-b */
}
void float_eg() {
	int x = 3490593;
	float f = (float) x;
	printf("For x = %d\n", x);
	show_int(x);
	show_float(f);
	
	x = 3510593;
	f = (float) x;
	printf("For x = %d\n", x);
	show_int(x);
	show_float(f);

}
int main(){
	int data = 255;
	//test_show_bytes(data);
	simple_show_a();
	//float_eg();
	return 0;
}
