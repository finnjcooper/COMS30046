#include <stddef.h>
#include <stdint.h>
#include <riscv_vector.h>

#define N 128
#define INT32_ASC(base) \
	(base) + 0, (base) + 1, (base) + 2, (base) + 3, \
	(base) + 4, (base) + 5, (base) + 6, (base) + 7, \
	(base) + 8, (base) + 9, (base) + 10, (base) + 11, \
	(base) + 12, (base) + 13, (base) + 14, (base) + 15, \
	(base) + 16, (base) + 17, (base) + 18, (base) + 19, \
	(base) + 20, (base) + 21, (base) + 22, (base) + 23, \
	(base) + 24, (base) + 25, (base) + 26, (base) + 27, \
	(base) + 28, (base) + 29, (base) + 30, (base) + 31
#define INT32_DESC(base) \
	(base) - 0, (base) - 1, (base) - 2, (base) - 3, \
	(base) - 4, (base) - 5, (base) - 6, (base) - 7, \
	(base) - 8, (base) - 9, (base) - 10, (base) - 11, \
	(base) - 12, (base) - 13, (base) - 14, (base) - 15, \
	(base) - 16, (base) - 17, (base) - 18, (base) - 19, \
	(base) - 20, (base) - 21, (base) - 22, (base) - 23, \
	(base) - 24, (base) - 25, (base) - 26, (base) - 27, \
	(base) - 28, (base) - 29, (base) - 30, (base) - 31

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int32_t a[N] = {
	INT32_ASC(1),
	INT32_ASC(33),
	INT32_ASC(65),
	INT32_ASC(97)
};
static int32_t b[N] = {
	INT32_DESC(128),
	INT32_DESC(96),
	INT32_DESC(64),
	INT32_DESC(32)
};
static int32_t result;

NOINLINE int32_t dot_product(int32_t a[N], int32_t b[N], int n) {
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
	KEEP_ALIVE(result);

	return 0;
}
