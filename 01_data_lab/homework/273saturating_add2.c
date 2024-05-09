#include <stdio.h>
#include <limits.h>
#include <assert.h>
/* Addition that saturates to TMin or TMax */
int saturating_add(int x, int y){
	int sum = x + y;
	int mask = INT_MIN;

	//Determine whether addtion overflows
	int pos_over = !(x & mask) && !(y & mask) && (sum & mask);
	int neg_over = (x & mask) && (y & mask) && !(sum & mask);

	//Returns the calculation result based on the overflow flag
	((pos_over) && (sum = INT_MAX)) || ((neg_over) && (sum = INT_MIN));

	return sum;
}
int main(){
	assert(saturating_add(-1, INT_MIN) == INT_MIN);

	return 0;
}
