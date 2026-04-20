#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define N 5

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

BENCHMARK_NOINLINE int32_t dot_row(int32_t *a, int32_t *bt, int n) {
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

BENCHMARK_NOINLINE void mat_mul(int32_t a[N][N], int32_t bt[N][N], int32_t c[N][N]) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			c[i][j] = dot_row(a[i], bt[j], N);
		}
	}
}

static int32_t a[N][N] = {
	{1, 2, 3, 4, 5},
	{6, 7, 8, 9, 10},
	{11, 12, 13, 14, 15},
	{16, 17, 18, 19, 20},
	{21, 22, 23, 24, 25}
};
static int32_t bt[N][N] = {
	{25, 20, 15, 10, 5},
	{24, 19, 14, 9, 4},
	{23, 18, 13, 8, 3},
	{22, 17, 12, 7, 2},
	{21, 16, 11, 6, 1}
};
static int32_t c[N][N];

int main() {
	mat_mul(a, bt, c);
	BENCHMARK_KEEP_ALIVE(c);

	return 0;
}
