int main() {
	int a = 1, b = 2, c = 3, d = 4;

	for (int i = 0; i < 64; i++) {
		a = a + b;
		c = c + d;
		b = b + 1;
		d = d + 1;
	}

	return a + c;
}
