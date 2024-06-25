#define A_LEFT			0x2190
#define A_UP			0x2191
#define A_RIGHT			0x2192
#define A_DOWN			0x2193
#define A_UP_LEFT		0x2196
#define A_UP_RIGHT		0x2197
#define A_DOWN_LEFT		0x2199
#define A_DOWN_RIGHT	0x2198
#include <stdio.h>

int main() {
	printf("%c\n", A_LEFT);
	printf("%c\n", A_UP);
	printf("%c\n", A_RIGHT);
	printf("%c\n", A_DOWN);
	printf("%c\n", A_UP_LEFT);
	printf("%c\n", A_UP_RIGHT);
	printf("%c\n", A_DOWN_LEFT);
	printf("%c\n", A_DOWN_RIGHT);
	return 0;
}