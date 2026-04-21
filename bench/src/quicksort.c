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

static int partition(int32_t values[N], int low, int high) {
	int32_t pivot = values[high];
	int i = low - 1;
	int32_t tmp;

	for (int j = low; j < high; j++) {
		if (values[j] <= pivot) {
			i++;
			tmp = values[i];
			values[i] = values[j];
			values[j] = tmp;
		}
	}

	tmp = values[i + 1];
	values[i + 1] = values[high];
	values[high] = tmp;
	return i + 1;
}

NOINLINE void quicksort(int32_t values[N], int low, int high) {
	int stack_low[N];
	int stack_high[N];
	int top = 0;

	stack_low[top] = low;
	stack_high[top] = high;
	top++;

	while (top > 0) {
		top--;
		low = stack_low[top];
		high = stack_high[top];

		if (low < high) {
			int pivot = partition(values, low, high);

			if (pivot - 1 > low) {
				stack_low[top] = low;
				stack_high[top] = pivot - 1;
				top++;
			}

			if (pivot + 1 < high) {
				stack_low[top] = pivot + 1;
				stack_high[top] = high;
				top++;
			}
		}
	}
}

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

int main() {
	quicksort(values, 0, N - 1);
	KEEP_ALIVE(values);

	return 0;
}
