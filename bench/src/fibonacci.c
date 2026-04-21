#include <stdint.h>

#define N 4

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t inputs[N] = {12, 13, 14, 15};
static int32_t output[N];

NOINLINE int32_t fibonacci(int32_t n) {
	if (n <= 1) return n;
	return fibonacci(n - 1) + fibonacci(n - 2);
}

NOINLINE void fibonacci_batch(int32_t inputs[N], int32_t output[N]) {
	for (int i = 0; i < N; i++) {
		output[i] = fibonacci(inputs[i]);
	}
}

int main() {
	fibonacci_batch(inputs, output);
	KEEP_ALIVE(output);

	return 0;
}
