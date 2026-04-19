#define N 64

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

BENCHMARK_NOINLINE int dot_product(int a[N], int b[N], int n) {
	int sum = 0;

	for (int i = 0; i < n; i++) {
		sum += a[i] * b[i];
	}

	return sum;
}

int main() {
	int a[N];
	int b[N];
	int result;

	for (int i = 0; i < N; i++) {
		a[i] = i + 1;
		b[i] = N - i;
	}

	result = dot_product(a, b, N);
	BENCHMARK_KEEP_ALIVE(result);

	return 0;
}
