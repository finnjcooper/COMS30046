#define IN_H 6
#define IN_W 6
#define K 3
#define OUT_H (IN_H - K + 1)
#define OUT_W (IN_W - K + 1)

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

BENCHMARK_NOINLINE void conv2d(int input[IN_H][IN_W], int kernel[K][K], int output[OUT_H][OUT_W]) {
	for (int i = 0; i < OUT_H; i++) {
		for (int j = 0; j < OUT_W; j++) {
			output[i][j] = 0;
			for (int ki = 0; ki < K; ki++) {
				for (int kj = 0; kj < K; kj++) {
					output[i][j] += input[i + ki][j + kj] * kernel[ki][kj];
				}
			}
		}
	}
}

int main() {
	static int input[IN_H][IN_W] = {
		{1, 2, 3, 4, 5, 6},
		{7, 8, 9, 10, 11, 12},
		{13, 14, 15, 16, 17, 18},
		{19, 20, 21, 22, 23, 24},
		{25, 26, 27, 28, 29, 30},
		{31, 32, 33, 34, 35, 36}
	};

	static int kernel[K][K] = {
		{1, 0, -1},
		{1, 0, -1},
		{1, 0, -1}
	};

	static int output[OUT_H][OUT_W];

	conv2d(input, kernel, output);
	BENCHMARK_KEEP_ALIVE(output);

	return 0;
}
