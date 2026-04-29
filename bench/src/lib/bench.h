#ifdef __GNUC__
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value)
#endif

#define INT16_ASC(base) \
	(base) + 0, (base) + 1, (base) + 2, (base) + 3, \
	(base) + 4, (base) + 5, (base) + 6, (base) + 7, \
	(base) + 8, (base) + 9, (base) + 10, (base) + 11, \
	(base) + 12, (base) + 13, (base) + 14, (base) + 15
#define INT16_DESC(base) \
	(base) - 0, (base) - 1, (base) - 2, (base) - 3, \
	(base) - 4, (base) - 5, (base) - 6, (base) - 7, \
	(base) - 8, (base) - 9, (base) - 10, (base) - 11, \
	(base) - 12, (base) - 13, (base) - 14, (base) - 15

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

#define INPUT_ROW(row) { \
	(float)((row) * IN_W + ((row) + 1) * 0), \
	(float)((row) * IN_W + ((row) + 1) * 1), \
	(float)((row) * IN_W + ((row) + 1) * 2), \
	(float)((row) * IN_W + ((row) + 1) * 3), \
	(float)((row) * IN_W + ((row) + 1) * 4), \
	(float)((row) * IN_W + ((row) + 1) * 5), \
	(float)((row) * IN_W + ((row) + 1) * 6), \
	(float)((row) * IN_W + ((row) + 1) * 7), \
	(float)((row) * IN_W + ((row) + 1) * 8), \
	(float)((row) * IN_W + ((row) + 1) * 9), \
	(float)((row) * IN_W + ((row) + 1) * 10), \
	(float)((row) * IN_W + ((row) + 1) * 11), \
	(float)((row) * IN_W + ((row) + 1) * 12), \
	(float)((row) * IN_W + ((row) + 1) * 13), \
	(float)((row) * IN_W + ((row) + 1) * 14), \
	(float)((row) * IN_W + ((row) + 1) * 15), \
	(float)((row) * IN_W + ((row) + 1) * 16), \
	(float)((row) * IN_W + ((row) + 1) * 17) \
}

#define FLOAT32_ASC(base) \
	(float)((base) + 0), (float)((base) + 1), (float)((base) + 2), (float)((base) + 3), \
	(float)((base) + 4), (float)((base) + 5), (float)((base) + 6), (float)((base) + 7), \
	(float)((base) + 8), (float)((base) + 9), (float)((base) + 10), (float)((base) + 11), \
	(float)((base) + 12), (float)((base) + 13), (float)((base) + 14), (float)((base) + 15), \
	(float)((base) + 16), (float)((base) + 17), (float)((base) + 18), (float)((base) + 19), \
	(float)((base) + 20), (float)((base) + 21), (float)((base) + 22), (float)((base) + 23), \
	(float)((base) + 24), (float)((base) + 25), (float)((base) + 26), (float)((base) + 27), \
	(float)((base) + 28), (float)((base) + 29), (float)((base) + 30), (float)((base) + 31)
