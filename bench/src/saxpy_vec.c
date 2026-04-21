#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

#include <stddef.h>
#include <riscv_vector.h>

#define N 128
#define FLOAT32(base) \
	(float)((base) + 0), (float)((base) + 1), (float)((base) + 2), (float)((base) + 3), \
	(float)((base) + 4), (float)((base) + 5), (float)((base) + 6), (float)((base) + 7), \
	(float)((base) + 8), (float)((base) + 9), (float)((base) + 10), (float)((base) + 11), \
	(float)((base) + 12), (float)((base) + 13), (float)((base) + 14), (float)((base) + 15), \
	(float)((base) + 16), (float)((base) + 17), (float)((base) + 18), (float)((base) + 19), \
	(float)((base) + 20), (float)((base) + 21), (float)((base) + 22), (float)((base) + 23), \
	(float)((base) + 24), (float)((base) + 25), (float)((base) + 26), (float)((base) + 27), \
	(float)((base) + 28), (float)((base) + 29), (float)((base) + 30), (float)((base) + 31)

static float x[N] = {
	FLOAT32(0),
	FLOAT32(32),
	FLOAT32(64),
	FLOAT32(96)
};
static float y[N] = { [0 ... N - 1] = 1.0f };

NOINLINE void saxpy(float a, float x[N], float y[N], int n) {
	while (n > 0) {
		size_t vl = __riscv_vsetvl_e32m1(n);
		vfloat32m1_t vx = __riscv_vle32_v_f32m1(x, vl);
		vfloat32m1_t vy = __riscv_vle32_v_f32m1(y, vl);
		vfloat32m1_t va = __riscv_vfmv_v_f_f32m1(a, vl);
		vy = __riscv_vfmacc_vv_f32m1(vy, vx, va, vl);
		__riscv_vse32_v_f32m1(y, vy, vl);
		x += vl;
		y += vl;
		n -= (int)vl;
	}
}

int main() {
	float a = 2.0f;

	saxpy(a, x, y, N);
	KEEP_ALIVE(y);

	return 0;
}
