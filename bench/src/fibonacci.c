#include <stdint.h>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t input = 14;
static int32_t output;

NOINLINE int32_t fibonacci(int32_t n) {
	if (n <= 1) return n;
	return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
	output = fibonacci(input);
	KEEP_ALIVE(output);

	return 0;
}
