#include <stdio.h>
#include <assert.h>
#include <string.h>
/* Copy integer into buffer if space is available */
/* WARNING: The following code is buggy */
void copy_int(int val, void *buf, int maxbytes) {
	if (maxbytes - (int)sizeof(val) >= 0)
		memcpy(buf, (void *) &val, sizeof(val));
}
int main(){
	int maxbytes = 2;
	if (maxbytes - (int)sizeof(int) >= 0)
		printf("error\n");
	else
		printf("right\n");
	return 0;
}

