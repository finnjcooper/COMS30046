#define N 64

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int a[N] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 59, 60, 61, 62, 63, 64
};
static int b[N] = {
	64, 63, 62, 61, 60, 59, 58, 57,
	56, 55, 54, 53, 52, 51, 50, 49,
	48, 47, 46, 45, 44, 43, 42, 41,
	40, 39, 38, 37, 36, 35, 34, 33,
	32, 31, 30, 29, 28, 27, 26, 25,
	24, 23, 22, 21, 20, 19, 18, 17,
	16, 15, 14, 13, 12, 11, 10, 9,
	8, 7, 6, 5, 4, 3, 2, 1
};
static int result;

BENCHMARK_NOINLINE int dot_product_vec(int *x, int *y, int n) {
	int *px = x;
	int *py = y;
	int remaining = n;
	int sum = 0;

	while (remaining > 0) {
		int vl;
		int partial;
		__asm__ volatile(
			"vsetvli %[vl], %[remaining], e32, m1, ta, ma\n"
			"vle32.v v1, (%[px])\n"
			"vle32.v v2, (%[py])\n"
			"vmv.v.i v0, 0\n"
			"vmv.v.i v3, 0\n"
			"vmacc.vv v3, v1, v2\n"
			"vredsum.vs v3, v3, v0\n"
			"vmv.x.s %[partial], v3\n"
			: [vl] "=&r"(vl), [partial] "=&r"(partial)
			: [remaining] "r"(remaining), [px] "r"(px), [py] "r"(py)
			: "memory");
		sum += partial;
		px += vl;
		py += vl;
		remaining -= vl;
	}

	return sum;
}

int main() {
	result = dot_product_vec(a, b, N);
	BENCHMARK_KEEP_ALIVE(result);

	return 0;
}
