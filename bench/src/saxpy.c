#define N 64

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

BENCHMARK_NOINLINE void saxpy(float a, float x[N], float y[N], int n) {
	for (int i = 0; i < n; i++) {
		y[i] = a * x[i] + y[i];
	}
}

int main() {
	float a = 2.0f;
	float x[N];
	float y[N];

	for (int i = 0; i < N; i++) {
		x[i] = (float) i;
		y[i] = 1.0f;
	}

	saxpy(a, x, y, N);
	BENCHMARK_KEEP_ALIVE(y);

	return 0;
}
