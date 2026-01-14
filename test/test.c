int main() {
	int a[3] = {6, 7, 8};
	int b[3] = {4, 4, 4};
	int c[3];
	
	for (int i = 0; i < 3; i++) {
		c[i] = a[i] + b[i];
	}

	// int a = 6;
	// int b = 7;
	// int c = a + b;

    __asm__ volatile ("ebreak");

	return 0;
}