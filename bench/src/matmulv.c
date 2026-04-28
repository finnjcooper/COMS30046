#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define N 16
#define ROW16(base) { \
	(base) + 0, (base) + 1, (base) + 2, (base) + 3, \
	(base) + 4, (base) + 5, (base) + 6, (base) + 7, \
	(base) + 8, (base) + 9, (base) + 10, (base) + 11, \
	(base) + 12, (base) + 13, (base) + 14, (base) + 15 \
}
#define BT_ROW(row) { \
	256 - (row), 240 - (row), 224 - (row), 208 - (row), \
	192 - (row), 176 - (row), 160 - (row), 144 - (row), \
	128 - (row), 112 - (row), 96 - (row), 80 - (row), \
	64 - (row), 48 - (row), 32 - (row), 16 - (row) \
}

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

NOINLINE int32_t dot_row(int32_t *a, int32_t *bt, int n) {
	int32_t sum = 0;

	while (n > 0) {
		size_t vl = __riscv_vsetvl_e32m1(n);
		vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
		vint32m1_t vb = __riscv_vle32_v_i32m1(bt, vl);
		vint32m1_t product = __riscv_vmul_vv_i32m1(va, vb, vl);
		vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
		vint32m1_t partial = __riscv_vredsum_vs_i32m1_i32m1(product, zero, vl);
		sum += __riscv_vmv_x_s_i32m1_i32(partial);
		a += vl;
		bt += vl;
		n -= (int)vl;
	}

	return sum;
}

NOINLINE void mat_mul(int32_t a[N][N], int32_t bt[N][N], int32_t c[N][N]) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			c[i][j] = dot_row(a[i], bt[j], N);
		}
	}
}

static int32_t a[N][N] = {
	ROW16(1), ROW16(17), ROW16(33), ROW16(49),
	ROW16(65), ROW16(81), ROW16(97), ROW16(113),
	ROW16(129), ROW16(145), ROW16(161), ROW16(177),
	ROW16(193), ROW16(209), ROW16(225), ROW16(241)
};
static int32_t bt[N][N] = {
	BT_ROW(0), BT_ROW(1), BT_ROW(2), BT_ROW(3),
	BT_ROW(4), BT_ROW(5), BT_ROW(6), BT_ROW(7),
	BT_ROW(8), BT_ROW(9), BT_ROW(10), BT_ROW(11),
	BT_ROW(12), BT_ROW(13), BT_ROW(14), BT_ROW(15)
};
static int32_t c[N][N];

int main() {
	mat_mul(a, bt, c);
	KEEP_ALIVE(c);

	return 0;
}
