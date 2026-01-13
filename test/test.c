int main() {
	int a = 5;
	int b = 10;
	int result = a + b;

    __asm__ volatile ("ebreak");

	return result;
}