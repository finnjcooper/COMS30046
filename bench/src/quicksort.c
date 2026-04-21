#include <stdint.h>

#define N 64

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
	37, 12, 89, 4, 65, 28, 91, 7,
	42, 73, 18, 56, 99, 31, 2, 84,
	47, 23, 68, 15, 93, 6, 54, 40,
	81, 26, 70, 10, 59, 34, 96, 1,
	44, 78, 21, 63, 87, 30, 5, 52,
	75, 17, 61, 39, 90, 24, 13, 67,
	100, 33, 8, 49, 72, 19, 58, 86,
	27, 94, 11, 45, 69, 3, 80, 36
};

int main() {
	quicksort(values, 0, N - 1);
	KEEP_ALIVE(values);

	return 0;
}
