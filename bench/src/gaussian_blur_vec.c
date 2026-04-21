#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define IN_H 10
#define IN_W 10
#define K 3
#define OUT_H (IN_H - K + 1)
#define OUT_W (IN_W - K + 1)

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static float input[IN_H][IN_W] = {
	{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f},
	{10.0f, 12.0f, 14.0f, 16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 26.0f, 28.0f},
	{20.0f, 23.0f, 26.0f, 29.0f, 32.0f, 35.0f, 38.0f, 41.0f, 44.0f, 47.0f},
	{30.0f, 34.0f, 38.0f, 42.0f, 46.0f, 50.0f, 54.0f, 58.0f, 62.0f, 66.0f},
	{40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f, 75.0f, 80.0f, 85.0f},
	{50.0f, 56.0f, 62.0f, 68.0f, 74.0f, 80.0f, 86.0f, 92.0f, 98.0f, 104.0f},
	{60.0f, 67.0f, 74.0f, 81.0f, 88.0f, 95.0f, 102.0f, 109.0f, 116.0f, 123.0f},
	{70.0f, 78.0f, 86.0f, 94.0f, 102.0f, 110.0f, 118.0f, 126.0f, 134.0f, 142.0f},
	{80.0f, 89.0f, 98.0f, 107.0f, 116.0f, 125.0f, 134.0f, 143.0f, 152.0f, 161.0f},
	{90.0f, 100.0f, 110.0f, 120.0f, 130.0f, 140.0f, 150.0f, 160.0f, 170.0f, 180.0f}
};
static float kernel[K][K] = {
	{0.0625f, 0.125f, 0.0625f},
	{0.125f, 0.25f, 0.125f},
	{0.0625f, 0.125f, 0.0625f}
};
static float output[OUT_H][OUT_W];

NOINLINE void gaussian_blur(float input[IN_H][IN_W], float output[OUT_H][OUT_W]) {
	for (int i = 0; i < OUT_H; i++) {
		int j = 0;
		while (j < OUT_W) {
			size_t vl = __riscv_vsetvl_e32m1(OUT_W - j);
			vfloat32m1_t sum = __riscv_vfmv_v_f_f32m1(0.0f, vl);

			for (int ki = 0; ki < K; ki++) {
				for (int kj = 0; kj < K; kj++) {
					vfloat32m1_t pixels = __riscv_vle32_v_f32m1(&input[i + ki][j + kj], vl);
					vfloat32m1_t coeff = __riscv_vfmv_v_f_f32m1(kernel[ki][kj], vl);
					sum = __riscv_vfmacc_vv_f32m1(sum, pixels, coeff, vl);
				}
			}

			__riscv_vse32_v_f32m1(&output[i][j], sum, vl);
			j += (int)vl;
		}
	}
}

int main() {
	gaussian_blur(input, output);
	KEEP_ALIVE(output);

	return 0;
}
