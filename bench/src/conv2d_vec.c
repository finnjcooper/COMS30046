#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

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

BENCHMARK_NOINLINE int32_t dot3(int32_t *input, int32_t *kernel) {
	size_t vl = __riscv_vsetvl_e32m1(K);
	vint32m1_t vi = __riscv_vle32_v_i32m1(input, vl);
	vint32m1_t vk = __riscv_vle32_v_i32m1(kernel, vl);
	vint32m1_t product = __riscv_vmul_vv_i32m1(vi, vk, vl);
	vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
	vint32m1_t partial = __riscv_vredsum_vs_i32m1_i32m1(product, zero, vl);
	return __riscv_vmv_x_s_i32m1_i32(partial);
}

BENCHMARK_NOINLINE void conv2d(int32_t input[IN_H][IN_W], int32_t kernel[K][K], int32_t output[OUT_H][OUT_W]) {
	for (int i = 0; i < OUT_H; i++) {
		for (int j = 0; j < OUT_W; j++) {
			output[i][j] = 0;
			for (int ki = 0; ki < K; ki++) {
				output[i][j] += dot3(&input[i + ki][j], kernel[ki]);
			}
		}
	}
}

static int32_t input[IN_H][IN_W] = {
	{1, 2, 3, 4, 5, 6},
	{7, 8, 9, 10, 11, 12},
	{13, 14, 15, 16, 17, 18},
	{19, 20, 21, 22, 23, 24},
	{25, 26, 27, 28, 29, 30},
	{31, 32, 33, 34, 35, 36}
};
static int32_t kernel[K][K] = {
	{1, 0, -1},
	{1, 0, -1},
	{1, 0, -1}
};
static int32_t output[OUT_H][OUT_W];

int main() {
	conv2d(input, kernel, output);
	BENCHMARK_KEEP_ALIVE(output);

	return 0;
}
