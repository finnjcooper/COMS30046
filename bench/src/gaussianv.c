#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>
#include "lib/bench.h"

#define IN_H 18
#define IN_W 18
#define K 3
#define OUT_H (IN_H - K + 1)
#define OUT_W (IN_W - K + 1)

static float input[IN_H][IN_W] = {
	INPUT_ROW(0), INPUT_ROW(1), INPUT_ROW(2), INPUT_ROW(3),
	INPUT_ROW(4), INPUT_ROW(5), INPUT_ROW(6), INPUT_ROW(7),
	INPUT_ROW(8), INPUT_ROW(9), INPUT_ROW(10), INPUT_ROW(11),
	INPUT_ROW(12), INPUT_ROW(13), INPUT_ROW(14), INPUT_ROW(15),
	INPUT_ROW(16), INPUT_ROW(17)
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
					sum = __riscv_vfmacc_vf_f32m1(sum, kernel[ki][kj], pixels, vl);
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
