#include <stdio.h>
#include <assert.h>
#include <limits.h>
/* Addition that saturates to TMin or TMax */
int saturating_add(int x, int y){
	int sig_mask = INT_MIN;
	int sum = x + y;

	/*if x > 0,y > 0, sum < 0 positive overflow
		if x < 0,y < 0, sum > 0 negative overflow
	*/
	int pos_over = !(x & sig_mask) && !(y & sig_mask) && (sum & sig_mask);
	int neg_over = (x & sig_mask) && (y & sig_mask) && !(sum & sig_mask);

	(pos_over && (sum = INT_MAX)) || (neg_over && (sum = INT_MIN));
	return sum;
}

int main(){
	int x = -1;
	int y = INT_MIN;
	assert(saturating_add(x, y) == INT_MIN);
	assert(saturating_add(x, 289) == 288);
	assert(saturating_add(123,INT_MAX) == INT_MAX);
	return 0;
}
