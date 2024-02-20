#include <stdio.h>
#include <assert.h>
#include <limits.h>
/* Determine whether arguments can be subtracted without overflow */
int tsub_ok(int x, int y){
	int sub_result = x - y;
	int res = 1;
	(y == INT_MIN) && (res = 0);

	int pos_over = x > 0 && y < 0 && sub_result < 0;
	int neg_over = x < 0 && y > 0 && sub_result > 0;

	return res && !(pos_over || neg_over);
}
int main(){
	assert(tsub_ok(1, INT_MIN) == 0);
	return 0;
}
