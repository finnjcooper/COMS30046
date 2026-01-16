int main() {
	int a[3] = {6, 7, 8};
	int b[3] = {4, 4, 4};
	int c[3];
	
	for (int i = 0; i < 3; i++) {
		c[i] = a[i] + b[i];
	}

	return c[2];
}