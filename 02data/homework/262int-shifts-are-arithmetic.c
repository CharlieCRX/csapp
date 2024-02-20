#include <stdio.h>
int int_shifts_are_arithmetic(){
	int testNum = -1;
	int shiftedRightOneInt = testNum >> (sizeof(int) << 3);
	if(shiftedRightOneInt == -1){
		return 1;
	}else if(shiftedRightOneInt == 0){
		return 0;
	}else{
		return -1;
	}
}
int main(){
	printf("int_shifts_are_arithmetic() = %d\n",int_shifts_are_arithmetic());
	return 0;
}
