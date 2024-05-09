#include <stdio.h>
int get_combination(int x, int y){
	int lx = x & 0xff;
	int ry = y & (~ 0xff);
	return lx | ry;
}

int main(){
	int x = -1;
	int y = 0x89674521;
	int cxy = get_combination(x, y);
	printf("0x%x\n",cxy);
	return 0;
}
