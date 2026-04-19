#define N 12

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

BENCHMARK_NOINLINE int factorial(int n) {
	int result = 1;

	for (int i = 2; i <= n; i++) {
		result *= i;
	}

	return result;
}

BENCHMARK_NOINLINE void factorial_range(int output[N]) {
	for (int i = 0; i < N; i++) {
		output[i] = factorial(i + 1);
	}
}

int main() {
	int output[N];

	factorial_range(output);
	BENCHMARK_KEEP_ALIVE(output);

	return 0;
}
