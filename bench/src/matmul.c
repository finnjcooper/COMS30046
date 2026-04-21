#include <stdint.h>

#define N 8

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

NOINLINE void mat_mul(int32_t a[N][N], int32_t bt[N][N], int32_t c[N][N]) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			c[i][j] = 0;
			for (int k = 0; k < N; k++)
				c[i][j] += a[i][k] * bt[j][k];
		}
	}
}

static int32_t a[N][N] = {
	{1, 2, 3, 4, 5, 6, 7, 8},
	{9, 10, 11, 12, 13, 14, 15, 16},
	{17, 18, 19, 20, 21, 22, 23, 24},
	{25, 26, 27, 28, 29, 30, 31, 32},
	{33, 34, 35, 36, 37, 38, 39, 40},
	{41, 42, 43, 44, 45, 46, 47, 48},
	{49, 50, 51, 52, 53, 54, 55, 56},
	{57, 58, 59, 60, 61, 62, 63, 64}
};
static int32_t bt[N][N] = {
	{64, 56, 48, 40, 32, 24, 16, 8},
	{63, 55, 47, 39, 31, 23, 15, 7},
	{62, 54, 46, 38, 30, 22, 14, 6},
	{61, 53, 45, 37, 29, 21, 13, 5},
	{60, 52, 44, 36, 28, 20, 12, 4},
	{59, 51, 43, 35, 27, 19, 11, 3},
	{58, 50, 42, 34, 26, 18, 10, 2},
	{57, 49, 41, 33, 25, 17, 9, 1}
};
static int32_t c[N][N];

int main() {
	mat_mul(a, bt, c);
	KEEP_ALIVE(c);

	return 0;
}
