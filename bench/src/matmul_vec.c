#define N 8

#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static int a[N * N] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	2, 3, 4, 5, 6, 7, 8, 9,
	3, 4, 5, 6, 7, 8, 9, 10,
	4, 5, 6, 7, 8, 9, 10, 11,
	5, 6, 7, 8, 9, 10, 11, 12,
	6, 7, 8, 9, 10, 11, 12, 13,
	7, 8, 9, 10, 11, 12, 13, 14,
	8, 9, 10, 11, 12, 13, 14, 15
};
static int bt[N * N] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	2, 3, 4, 5, 6, 7, 8, 9,
	3, 4, 5, 6, 7, 8, 9, 10,
	4, 5, 6, 7, 8, 9, 10, 11,
	5, 6, 7, 8, 9, 10, 11, 12,
	6, 7, 8, 9, 10, 11, 12, 13,
	7, 8, 9, 10, 11, 12, 13, 14,
	8, 9, 10, 11, 12, 13, 14, 15
};
static int c[N * N];

BENCHMARK_NOINLINE void matmul_vec(int *a, int *bt, int *c, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int *pa = &a[i * n];
			int *pb = &bt[j * n];
			int remaining = n;
			int sum = 0;

			while (remaining > 0) {
				int vl;
				int partial;
				__asm__ volatile(
					"vsetvli %[vl], %[remaining], e32, m1, ta, ma\n"
					"vle32.v v1, (%[pa])\n"
					"vle32.v v2, (%[pb])\n"
					"vmv.v.i v0, 0\n"
					"vmv.v.i v3, 0\n"
					"vmacc.vv v3, v1, v2\n"
					"vredsum.vs v3, v3, v0\n"
					"vmv.x.s %[partial], v3\n"
					: [vl] "=&r"(vl), [partial] "=&r"(partial)
					: [remaining] "r"(remaining), [pa] "r"(pa), [pb] "r"(pb)
					: "memory");
				sum += partial;
				pa += vl;
				pb += vl;
				remaining -= vl;
			}

			c[i * n + j] = sum;
		}
	}
}

int main() {
	matmul_vec(a, bt, c, N);
	BENCHMARK_KEEP_ALIVE(c);

	return 0;
}
