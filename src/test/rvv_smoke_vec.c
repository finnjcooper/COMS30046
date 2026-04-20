#if defined(__GNUC__)
#define BENCHMARK_NOINLINE __attribute__((noinline, noclone))
#define BENCHMARK_KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define BENCHMARK_NOINLINE
#define BENCHMARK_KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static unsigned char in8[16] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16
};
static unsigned char out8[16];

static unsigned short in16[16] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16
};
static unsigned short out16[16];

static int in32[16] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16
};
static int out32[16];

BENCHMARK_NOINLINE void smoke_e8(unsigned char *input, unsigned char *output) {
	__asm__ volatile(
		"vsetivli zero, 16, e8, m1, ta, ma\n"
		"vle8.v v1, (%[input])\n"
		"vadd.vi v1, v1, 1\n"
		"vse8.v v1, (%[output])\n"
		:
		: [input] "r"(input), [output] "r"(output)
		: "memory");
}

BENCHMARK_NOINLINE void smoke_e16(unsigned short *input, unsigned short *output) {
	int scale = 3;
	__asm__ volatile(
		"vsetivli zero, 16, e16, m1, ta, ma\n"
		"vle16.v v1, (%[input])\n"
		"vmul.vx v1, v1, %[scale]\n"
		"vse16.v v1, (%[output])\n"
		:
		: [input] "r"(input), [output] "r"(output), [scale] "r"(scale)
		: "memory");
}

BENCHMARK_NOINLINE void smoke_e32(int *input, int *output) {
	int n = 16;
	int vl;
	__asm__ volatile(
		"vsetvli %[vl], %[n], e32, m1, ta, ma\n"
		"vle32.v v1, (%[input])\n"
		"vmv.v.i v2, 5\n"
		"vadd.vv v3, v1, v2\n"
		"vse32.v v3, (%[output])\n"
		: [vl] "=&r"(vl)
		: [n] "r"(n), [input] "r"(input), [output] "r"(output)
		: "memory");
}

int main() {
	smoke_e8(in8, out8);
	smoke_e16(in16, out16);
	smoke_e32(in32, out32);
	BENCHMARK_KEEP_ALIVE(out8);
	BENCHMARK_KEEP_ALIVE(out16);
	BENCHMARK_KEEP_ALIVE(out32);

	return 0;
}
