#include <immintrin.h>
#include <iostream>
#include <thread>

using namespace std;

void convolve(double** source, double** dest, int source_height, int source_width, double** filter, int filter_height, int filter_width) {
	for (int s_h = 0; s_h < source_height - (filter_height - 1); s_h++) {
		for (int s_w = 0; s_w < source_width - (filter_width - 1); s_w++) {
			__m256d total_sum = _mm256_set_pd(0.0, 0.0, 0.0, 0.0);
			for (int f_h = 0; f_h < filter_height; f_h++) {
				for (int f_w = 0; f_w < filter_width; f_w += 4) {
					__m256d source_vec = _mm256_load_pd(source[s_h + f_h] + s_w + f_w);
					__m256d filter_vec = _mm256_load_pd(filter[f_h] + f_w);
					__m256d prod = _mm256_mul_pd(source_vec, filter_vec);
					switch (filter_width - f_w)
					{
					case 1:
						((double*)&prod)[1] = 0;
					case 2:
						((double*)&prod)[2] = 0;
					case 3:
						((double*)&prod)[3] = 0;
					default:
						break;
					}
					__m256d sum = _mm256_hadd_pd(prod, total_sum);
					__m256d sum_high = _mm256_castpd128_pd256(_mm256_extractf128_pd(sum, 1));
					total_sum = _mm256_add_pd(sum, sum_high);
				}
			}
			dest[s_h][s_w] = ((double*)&total_sum)[0];
		}
	}
}

void creat_matrix(double**& matrix, int height, int width) {
	matrix = new double* [height];
	for (int i = 0; i < height; i++) {
		matrix[i] = new double[width];
	}
}

void random_fill_matrix(double** matrix, int height, int width) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			matrix[i][j] = (rand() % 1000) / 1000.0;
		}
	}
}

void parallel_random_fill_matrix(double** matrix, int height, int width) {
	unsigned int thread_count = thread::hardware_concurrency();
	thread* threads = new thread[thread_count];

	for (int i = 0; i < thread_count; i++) {
		threads[i] = thread(random_fill_matrix, matrix + i * (height / thread_count), height / thread_count, width);
	}

	for (int i = 0; i < thread_count; i++) {
		threads[i].join();
	}
}

int main() {
	const int source_height = 16384;
	const int source_width = 16384;
	const int filter_height = 15; // always odd.
	const int filter_width = 15; // always odd.
	
	int padding = 4 - filter_width % 4; // pad to make filter multiple of 4 to prevent illegal memory access 

	double** filter = nullptr;
	double** source = nullptr;
	double** dest = nullptr;

	creat_matrix(filter, filter_height, filter_width + padding);
	random_fill_matrix(filter, filter_height, filter_width);
	creat_matrix(source, source_height, source_width + padding);
	parallel_random_fill_matrix(source, source_height, source_width);
	creat_matrix(dest, source_height - filter_height + 1, source_width - filter_width + 1);

	cout << "Convolution Started..." << endl;

	unsigned int thread_count = thread::hardware_concurrency();

	thread* threads = new thread[thread_count];

	int placements = source_height - (filter_height - 1);
	int placements_per_thread = placements / thread_count;

	for (int i = 0; i < thread_count; i++) {
		threads[i] = thread(convolve, source + i * placements_per_thread, dest, placements_per_thread + (filter_height - 1),
			source_width, filter, filter_height, filter_width);
	}

	for (int i = 0; i < thread_count; i++) {
		threads[i].join();
	}

	cout << "Convolution complete" << endl;

	return 0;
}