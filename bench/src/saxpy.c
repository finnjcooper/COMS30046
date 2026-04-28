#define N 1024
#define FLOAT32(base) \
	(float)((base) + 0), (float)((base) + 1), (float)((base) + 2), (float)((base) + 3), \
	(float)((base) + 4), (float)((base) + 5), (float)((base) + 6), (float)((base) + 7), \
	(float)((base) + 8), (float)((base) + 9), (float)((base) + 10), (float)((base) + 11), \
	(float)((base) + 12), (float)((base) + 13), (float)((base) + 14), (float)((base) + 15), \
	(float)((base) + 16), (float)((base) + 17), (float)((base) + 18), (float)((base) + 19), \
	(float)((base) + 20), (float)((base) + 21), (float)((base) + 22), (float)((base) + 23), \
	(float)((base) + 24), (float)((base) + 25), (float)((base) + 26), (float)((base) + 27), \
	(float)((base) + 28), (float)((base) + 29), (float)((base) + 30), (float)((base) + 31)

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline, noclone))
#define KEEP_ALIVE(value) __asm__ volatile("" : : "m"(value) : "memory")
#else
#define NOINLINE
#define KEEP_ALIVE(value) do { (void)sizeof(value); } while (0)
#endif

static float x[N] = {
	FLOAT32(0),   FLOAT32(32),  FLOAT32(64),  FLOAT32(96),
	FLOAT32(128), FLOAT32(160), FLOAT32(192), FLOAT32(224),
	FLOAT32(256), FLOAT32(288), FLOAT32(320), FLOAT32(352),
	FLOAT32(384), FLOAT32(416), FLOAT32(448), FLOAT32(480),
	FLOAT32(512), FLOAT32(544), FLOAT32(576), FLOAT32(608),
	FLOAT32(640), FLOAT32(672), FLOAT32(704), FLOAT32(736),
	FLOAT32(768), FLOAT32(800), FLOAT32(832), FLOAT32(864),
	FLOAT32(896), FLOAT32(928), FLOAT32(960), FLOAT32(992)
};

static float y[N] = { [0 ... N - 1] = 1.0f };

NOINLINE void saxpy(float a, float x[N], float y[N], int n) {
	for (int i = 0; i < n; i++) {
		y[i] = a * x[i] + y[i];
	}
}

int main() {
	float a = 2.0f;

	saxpy(a, x, y, N);
	KEEP_ALIVE(y);

	return 0;
}
