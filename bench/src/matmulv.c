#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>
#include "lib/bench.h"

#define N 16

static int32_t a[N][N] = {
	{ INT16_ASC(1)   }, { INT16_ASC(17)  }, { INT16_ASC(33)  }, { INT16_ASC(49)  },
	{ INT16_ASC(65)  }, { INT16_ASC(81)  }, { INT16_ASC(97)  }, { INT16_ASC(113) },
	{ INT16_ASC(129) }, { INT16_ASC(145) }, { INT16_ASC(161) }, { INT16_ASC(177) },
	{ INT16_ASC(193) }, { INT16_ASC(209) }, { INT16_ASC(225) }, { INT16_ASC(241) }
};

static int32_t b[N][N] = {
	{ INT16_DESC(256) }, { INT16_DESC(240) }, { INT16_DESC(224) }, { INT16_DESC(208) },
	{ INT16_DESC(192) }, { INT16_DESC(176) }, { INT16_DESC(160) }, { INT16_DESC(144) },
	{ INT16_DESC(128) }, { INT16_DESC(112) }, { INT16_DESC(96)  }, { INT16_DESC(80)  },
	{ INT16_DESC(64)  }, { INT16_DESC(48)  }, { INT16_DESC(32)  }, { INT16_DESC(16)  }
};

static int32_t c[N][N];

NOINLINE void matmul(int32_t a[N][N], int32_t b[N][N], int32_t c[N][N]) {
	for (int32_t i = 0; i < N; i++) {
		for (int32_t j = 0; j < N;) {
			size_t vl = __riscv_vsetvl_e32m1(N - j);
			vint32m1_t acc = __riscv_vmv_v_x_i32m1(0, vl);
			for (int32_t k = 0; k < N; k++) {
				vint32m1_t b_row = __riscv_vle32_v_i32m1(&b[k][j], vl);
				acc = __riscv_vmacc_vx_i32m1(acc, a[i][k], b_row, vl);
			}
			__riscv_vse32_v_i32m1(&c[i][j], acc, vl);
			j += (int32_t) vl;
		}
	}
}

int main() {
	matmul(a, b, c);

	KEEP_ALIVE(c);
	return 0;
}
