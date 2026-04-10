int main() {
	volatile int a = 6;
	volatile int b = 4;
	volatile int c = a + b;

	return c;
}
