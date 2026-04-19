#define N 8

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int partition(int values[N], int low, int high) {
	int pivot = values[high];
	int i = low - 1;
	int tmp;

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

BENCHMARK_NOINLINE void quicksort(int values[N], int low, int high) {
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

int main() {
	int values[N] = {
		37, 12, 89, 4,
		65, 28, 91, 7
	};

	quicksort(values, 0, N - 1);
	BENCHMARK_KEEP_ALIVE(values);

	return 0;
}
