#include <stdint.h>

#define N 64

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t values[N] = {
	37, 12, 89, 4, 65, 28, 91, 7,
	42, 73, 18, 56, 99, 31, 2, 84,
	47, 23, 68, 15, 93, 6, 54, 40,
	81, 26, 70, 10, 59, 34, 96, 1,
	44, 78, 21, 63, 87, 30, 5, 52,
	75, 17, 61, 39, 90, 24, 13, 67,
	100, 33, 8, 49, 72, 19, 58, 86,
	27, 94, 11, 45, 69, 3, 80, 36
};

NOINLINE void insertion_sort(int32_t values[N]) {
	for (int i = 1; i < N; i++) {
		int32_t key = values[i];
		int j = i - 1;

		while (j >= 0 && values[j] > key) {
			values[j + 1] = values[j];
			j--;
		}

		values[j + 1] = key;
	}
}

int main() {
	insertion_sort(values);
	KEEP_ALIVE(values);

	return 0;
}
