#include <stdio.h>

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start,size_t len){
	size_t i;
  printf("len = %zu\n",len);
	for (i = 0; i < len; i++)
			printf(" %.2x", start[i]);    //line:data:show_bytes_printf
	printf("\n");
	for(i = 0; i < len; i++){
			printf("Byte %zu (Address: %p) 0x%.2x\n", i, start + i, start[i]);
		}
	printf("\n");
}

void show_int (int x){
	show_bytes((byte_pointer) &x,sizeof(int));
}

void show_float(float x){
	show_bytes((byte_pointer) &x,sizeof(float));
}

void show_double(double x){
	show_bytes((byte_pointer) &x,sizeof(double));
}

void show_pointer(void *x){
	show_bytes((byte_pointer) &x,sizeof(void *));
}

/* $begin test-show-bytes */
void test_show_bytes(int val) {
	printf("(decimal) val = %d.\n",val);
	printf("(hexadecimal) val = %x.\n",val);

	int ival = val;
	float fval = (float) ival;
	int *pval = &ival;
	show_int(ival);
	show_float(fval);
	show_pointer(pval);
}
/* $end test-show-bytes */
void show_twocomp() 
{
	/* $begin show-twocomp */
	    short x = 12345; 
			short mx = -x; 
					    
			show_bytes((byte_pointer) &x, sizeof(short)); 
			show_bytes((byte_pointer) &mx, sizeof(short)); 
	/* $end show-twocomp */
}
void show_cast()
{
	int a = 65534;
	double b = a;
	show_int(a);
	show_double(b);
}

int main(){
	//int a = 0x12345678;
	//int a = 12345;
	//test_show_bytes(a);
	//char str[6] = "12345";
	//show_bytes(str,6);
	//show_twocomp();
	show_cast();
	return 0;
}
