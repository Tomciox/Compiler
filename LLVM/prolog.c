#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readInt() {
	int n;
	scanf("%d", &n);
	return n;
}

void printInt(int n) {
	printf("%d\n", n);
}

const int BUFFER_SIZE = 256;

char *readString() {
	char *n = malloc(BUFFER_SIZE * sizeof(char));
	scanf("%s", n);
	return n;
}

void printString(char *s) {
	printf("%s\n", s);
}

char* concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void error() {
	printf("runtime error\n");
	exit(1);
}

// usunąć z .ll
int main() {
	char *S1 = "", *S2 = "";
	int d = strcmp(S1, S2);
	return 0;
}


