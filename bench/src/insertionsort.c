#include <stdint.h>

#define N 128
#define VALUE(index) ((((index) * 37 + 23) & 127) + 1)
#define VALUES16(base) \
	VALUE((base) + 0), VALUE((base) + 1), VALUE((base) + 2), VALUE((base) + 3), \
	VALUE((base) + 4), VALUE((base) + 5), VALUE((base) + 6), VALUE((base) + 7), \
	VALUE((base) + 8), VALUE((base) + 9), VALUE((base) + 10), VALUE((base) + 11), \
	VALUE((base) + 12), VALUE((base) + 13), VALUE((base) + 14), VALUE((base) + 15)

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t values[N] = {
	VALUES16(0),
	VALUES16(16),
	VALUES16(32),
	VALUES16(48),
	VALUES16(64),
	VALUES16(80),
	VALUES16(96),
	VALUES16(112)
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
