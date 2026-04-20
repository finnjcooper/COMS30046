#define IN 8
#define K 3
#define OUT (IN - K + 1)

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int input[IN * IN] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	2, 3, 4, 5, 6, 7, 8, 9,
	3, 4, 5, 6, 7, 8, 9, 10,
	4, 5, 6, 7, 8, 9, 10, 11,
	5, 6, 7, 8, 9, 10, 11, 12,
	6, 7, 8, 9, 10, 11, 12, 13,
	7, 8, 9, 10, 11, 12, 13, 14,
	8, 9, 10, 11, 12, 13, 14, 15
};
static int kernel[K * K] = {
	1, 0, -1,
	1, 0, -1,
	1, 0, -1
};
static int output[OUT * OUT];

BENCHMARK_NOINLINE void conv2d_vec(int *input, int *kernel, int *output) {
	for (int y = 0; y < OUT; y++) {
		for (int x = 0; x < OUT; x++) {
			int sum = 0;

			for (int ky = 0; ky < K; ky++) {
				int *in_row = &input[(y + ky) * IN + x];
				int *k_row = &kernel[ky * K];
				int partial;

				__asm__ volatile(
					"vsetivli zero, 3, e32, m1, ta, ma\n"
					"vle32.v v1, (%[in_row])\n"
					"vle32.v v2, (%[k_row])\n"
					"vmv.v.i v0, 0\n"
					"vmv.v.i v3, 0\n"
					"vmacc.vv v3, v1, v2\n"
					"vredsum.vs v3, v3, v0\n"
					"vmv.x.s %[partial], v3\n"
					: [partial] "=&r"(partial)
					: [in_row] "r"(in_row), [k_row] "r"(k_row)
					: "memory");
				sum += partial;
			}

			output[y * OUT + x] = sum;
		}
	}
}

int main() {
	conv2d_vec(input, kernel, output);
	BENCHMARK_KEEP_ALIVE(output);

	return 0;
}
