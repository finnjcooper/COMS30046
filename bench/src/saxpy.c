#include "lib/bench.h"

#define N 1024

static float x[N] = {
	FLOAT32_ASC(0),   FLOAT32_ASC(32),  FLOAT32_ASC(64),  FLOAT32_ASC(96),
	FLOAT32_ASC(128), FLOAT32_ASC(160), FLOAT32_ASC(192), FLOAT32_ASC(224),
	FLOAT32_ASC(256), FLOAT32_ASC(288), FLOAT32_ASC(320), FLOAT32_ASC(352),
	FLOAT32_ASC(384), FLOAT32_ASC(416), FLOAT32_ASC(448), FLOAT32_ASC(480),
	FLOAT32_ASC(512), FLOAT32_ASC(544), FLOAT32_ASC(576), FLOAT32_ASC(608),
	FLOAT32_ASC(640), FLOAT32_ASC(672), FLOAT32_ASC(704), FLOAT32_ASC(736),
	FLOAT32_ASC(768), FLOAT32_ASC(800), FLOAT32_ASC(832), FLOAT32_ASC(864),
	FLOAT32_ASC(896), FLOAT32_ASC(928), FLOAT32_ASC(960), FLOAT32_ASC(992)
};

static float y[N] = { [0 ... N - 1] = 1.0f };

static float a = 2.0f;

NOINLINE void saxpy(float a, float x[N], float y[N], int n) {
	for (int i = 0; i < n; i++) {
		y[i] = a * x[i] + y[i];
	}
}

int main() {
	saxpy(a, x, y, N);

	KEEP_ALIVE(y);
	return 0;
}
