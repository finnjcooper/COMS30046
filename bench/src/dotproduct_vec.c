#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define N 64

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t a[N] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 59, 60, 61, 62, 63, 64
};
static int32_t b[N] = {
	64, 63, 62, 61, 60, 59, 58, 57,
	56, 55, 54, 53, 52, 51, 50, 49,
	48, 47, 46, 45, 44, 43, 42, 41,
	40, 39, 38, 37, 36, 35, 34, 33,
	32, 31, 30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19, 18, 17,
	16, 15, 14, 13, 12, 11, 10, 9,
	8, 7, 6, 5, 4, 3, 2, 1
};
static int32_t result;

BENCHMARK_NOINLINE int32_t dot_product(int32_t a[N], int32_t b[N], int n) {
	int32_t sum = 0;

	while (n > 0) {
		size_t vl = __riscv_vsetvl_e32m1(n);
		vint32m1_t va = __riscv_vle32_v_i32m1(a, vl);
		vint32m1_t vb = __riscv_vle32_v_i32m1(b, vl);
		vint32m1_t product = __riscv_vmul_vv_i32m1(va, vb, vl);
		vint32m1_t zero = __riscv_vmv_v_x_i32m1(0, vl);
		vint32m1_t partial = __riscv_vredsum_vs_i32m1_i32m1(product, zero, vl);
		sum += __riscv_vmv_x_s_i32m1_i32(partial);
		a += vl;
		b += vl;
		n -= (int)vl;
	}

	return sum;
}

int main() {
	result = dot_product(a, b, N);
	BENCHMARK_KEEP_ALIVE(result);

	return 0;
}
