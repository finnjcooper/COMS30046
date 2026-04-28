#include <stdint.h>
#include "lib/optim.h"

#define N 1024
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

static int32_t a[N] = {
	INT32_ASC(1),   INT32_ASC(33),  INT32_ASC(65),  INT32_ASC(97),
	INT32_ASC(129), INT32_ASC(161), INT32_ASC(193), INT32_ASC(225),
	INT32_ASC(257), INT32_ASC(289), INT32_ASC(321), INT32_ASC(353),
	INT32_ASC(385), INT32_ASC(417), INT32_ASC(449), INT32_ASC(481),
	INT32_ASC(513), INT32_ASC(545), INT32_ASC(577), INT32_ASC(609),
	INT32_ASC(641), INT32_ASC(673), INT32_ASC(705), INT32_ASC(737),
	INT32_ASC(769), INT32_ASC(801), INT32_ASC(833), INT32_ASC(865),
	INT32_ASC(897), INT32_ASC(929), INT32_ASC(961), INT32_ASC(993)
};

static int32_t b[N] = {
	INT32_DESC(1024), INT32_DESC(992), INT32_DESC(960), INT32_DESC(928),
	INT32_DESC(896),  INT32_DESC(864), INT32_DESC(832), INT32_DESC(800),
	INT32_DESC(768),  INT32_DESC(736), INT32_DESC(704), INT32_DESC(672), 
	INT32_DESC(640),  INT32_DESC(608), INT32_DESC(576), INT32_DESC(544),
	INT32_DESC(512),  INT32_DESC(480), INT32_DESC(448), INT32_DESC(416),
	INT32_DESC(384),  INT32_DESC(352), INT32_DESC(320), INT32_DESC(288),
	INT32_DESC(256),  INT32_DESC(224), INT32_DESC(192), INT32_DESC(160),
	INT32_DESC(128),  INT32_DESC(96),  INT32_DESC(64),  INT32_DESC(32)
};

static int32_t c;

NOINLINE int32_t dot_product(int32_t a[N], int32_t b[N], int n) {
	int32_t sum = 0;

	for (int i = 0; i < n; i++) {
		sum += a[i] * b[i];
	}

	return sum;
}

int main() {
	c = dot_product(a, b, N);
	
	KEEP_ALIVE(c);
	return 0;
}
