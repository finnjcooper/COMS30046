#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

#include <stddef.h>
#include <riscv_vector.h>

#define N 64

static float x[N] = {
	0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f,
	8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,
	16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f,
	24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f, 30.0f, 31.0f,
	32.0f, 33.0f, 34.0f, 35.0f, 36.0f, 37.0f, 38.0f, 39.0f,
	40.0f, 41.0f, 42.0f, 43.0f, 44.0f, 45.0f, 46.0f, 47.0f,
	48.0f, 49.0f, 50.0f, 51.0f, 52.0f, 53.0f, 54.0f, 55.0f,
	56.0f, 57.0f, 58.0f, 59.0f, 60.0f, 61.0f, 62.0f, 63.0f
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
