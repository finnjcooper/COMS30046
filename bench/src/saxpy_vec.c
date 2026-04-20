#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

#include <stddef.h>
#include <riscv_vector.h>

#define N 64

BENCHMARK_NOINLINE void saxpy(float a, float x[N], float y[N], int n) {
	while (n > 0) {
		size_t vl = __riscv_vsetvl_e32m1(n);
		vfloat32m1_t vx = __riscv_vle32_v_f32m1(x, vl);
		vfloat32m1_t vy = __riscv_vle32_v_f32m1(y, vl);
		vy = __riscv_vfmacc_vf_f32m1(vy, a, vx, vl);
		__riscv_vse32_v_f32m1(y, vy, vl);
		x += vl;
		y += vl;
		n -= (int)vl;
	}
}

int main() {
	float a = 2.0f;
	float x[N];
	float y[N];

	for (int i = 0; i < N; i++) {
		x[i] = (float)i;
		y[i] = 1.0f;
	}

	saxpy(a, x, y, N);
	BENCHMARK_KEEP_ALIVE(y);

	return 0;
}
