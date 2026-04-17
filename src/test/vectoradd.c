int main() {
	volatile int a[10] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	volatile int b[10] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
	volatile int c[10];
	
	for (volatile int i = 0; i < 10; i++) {
		c[i] = a[i] + b[i];
	}

	return c[9];
}
