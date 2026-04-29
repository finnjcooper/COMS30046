#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>
#include "lib/bench.h"

#define N 1024

static int32_t a[N] = {
	INT32_ASC(1),   INT32_ASC(33),  INT32_ASC(65),  INT32_ASC(97),
	INT32_ASC(129), INT32_ASC(161), INT32_ASC(193), INT32_ASC(225),
	INT32_ASC(257), INT32_ASC(289), INT32_ASC(321), INT32_ASC(353),
	INT32_ASC(385), INT32_ASC(417), INT32_ASC(449), INT32_ASC(481),
	INT32_ASC(513), INT32_ASC(545), INT32_ASC(577), INT32_ASC(609),
	INT32_ASC(641), INT32_ASC(673), INT32_ASC(705), INT32_ASC(737),
	INT32_ASC(769), INT32_ASC(801), INT32_ASC(833), INT32_ASC(865),
	INT32_ASC(897), INT32_ASC(929), INT32_ASC(961), INT32_ASC(993)
};

static int32_t b[N] = {
	INT32_DESC(1024), INT32_DESC(992), INT32_DESC(960), INT32_DESC(928),
	INT32_DESC(896),  INT32_DESC(864), INT32_DESC(832), INT32_DESC(800),
	INT32_DESC(768),  INT32_DESC(736), INT32_DESC(704), INT32_DESC(672),
	INT32_DESC(640),  INT32_DESC(608), INT32_DESC(576), INT32_DESC(544),
	INT32_DESC(512),  INT32_DESC(480), INT32_DESC(448), INT32_DESC(416),
	INT32_DESC(384),  INT32_DESC(352), INT32_DESC(320), INT32_DESC(288),
	INT32_DESC(256),  INT32_DESC(224), INT32_DESC(192), INT32_DESC(160),
	INT32_DESC(128),  INT32_DESC(96),  INT32_DESC(64),  INT32_DESC(32)
};

static int32_t result;

NOINLINE int32_t dot_product(int32_t a[N], int32_t b[N], int n) {
	if (n <= 0)
		return 0;

	size_t vl = __riscv_vsetvl_e32m1(n);
	vint32m1_t acc = __riscv_vmv_v_x_i32m1(0, vl);

	while (n >= (int)vl) {
		vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
		vint32m1_t vb = __riscv_vle32_v_i32m1(b, vl);
		acc = __riscv_vmacc_vv_i32m1(acc, va, vb, vl);
		a += vl;
		b += vl;
		n -= (int)vl;
	}

	vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
	vint32m1_t partial = __riscv_vredsum_vs_i32m1_i32m1(acc, zero, vl);
	int32_t sum = __riscv_vmv_x_s_i32m1_i32(partial);

	for (int i = 0; i < n; i++)
		sum += a[i] * b[i];

	return sum;
}

int main() {
	result = dot_product(a, b, N);

	KEEP_ALIVE(result);
	return 0;
}
