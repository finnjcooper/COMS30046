int main() {
	volatile int n = 10, max = 5;
	for (volatile int i = 0; i < n; i++) {
		if (i > max) break;
	}

	return 0;
}
