#define N 64

void saxpy(float a, float* x, float* y, int n) {
	for (int i = 0; i < n; i++) {
		y[i] = a * x[i] + y[i];
	}
}

int main() {
	float a = 2.0f;
	float x[N];
	float y[N];

	for (int i = 0; i < N; i++) {
		x[i] = (float) i;
		y[i] = 1.0f;
	}

	saxpy(a, x, y, N);

	return 0;
}
