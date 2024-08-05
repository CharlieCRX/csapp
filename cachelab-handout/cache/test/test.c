#include <stdio.h>

int main() {
	FILE *file = fopen("example.txt", "r");
	if (!file) {
		perror("Error opening file");
		return 1;
	}

	char buffer[256];
	while (fgets(buffer, sizeof(buffer), file)) {
		printf("read line: %s",buffer);
	}

	fclose(file);
	return 0;

}
