#include <immintrin.h>
#include <iostream>
#include <thread>

using namespace std;

void convolve(double** source, double** dest, int source_height, int source_width, double** filter, int filter_height, int filter_width) {
	for (int s_h = 0; s_h < source_height - (filter_height - 1); s_h++) {
		for (int s_w = 0; s_w < source_width - (filter_width - 1); s_w++) {
			dest[s_h][s_w] = 0;
		}
	}
	for (int s_h = 0; s_h < source_height - (filter_height - 1); s_h++) {
		double* dest_base_address = dest[s_h];
		for (int f_h = 0; f_h < filter_height; f_h++) {
			double* source_base_address = source[s_h + f_h];
			double* source_end_width_address = source_base_address + (source_width - (filter_width - 1));
			double* filter_base_address = filter[f_h];
			double* dest_address = dest_base_address;
			for (; source_base_address < source_end_width_address; source_base_address++, dest_address++) {
				double total_sum = 0.0;
				double* source_filter_base_address = source_base_address;
				double* source_end_filter_width_address = source_filter_base_address + filter_width;
				double* filter_address = filter_base_address;
				for (; source_filter_base_address < source_end_filter_width_address; source_filter_base_address += 4, filter_address += 4) {
					__m256d source_vec = _mm256_load_pd(source_filter_base_address);
					__m256d filter_vec = _mm256_load_pd(filter_address);
					__m256d prod = _mm256_mul_pd(source_vec, filter_vec);
					__m256d sum = _mm256_hadd_pd(prod, prod);
					total_sum += ((double*)&sum)[0] + ((double*)&sum)[2];
				}
				(*dest_address) += total_sum;
			}
		}
	}
}

void creat_matrix(double**& matrix, int height, int width) {
	matrix = new double* [height];
	for (int i = 0; i < height; i++) {
		matrix[i] = new double[width];
	}
}

void random_fill_matrix_with_zero_padding(double** matrix, int height, int width, int padding_width, int padding_height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			matrix[i][j] = (rand() % 1000) / 1000.0;
		}
		for (int j = width; j < width + padding_width; j++) {
			matrix[i][j] = 0;
		}
	}
	for (int i = height; i < height + padding_height; i++) {
		for (int j = 0; j < width + padding_width; j++) {
			matrix[i][j] = 0;
		}
	}
}

void parallel_random_fill_matrix_with_zero_padding(double** matrix, int height, int width, int padding_width, int padding_height) {
	unsigned int thread_count = thread::hardware_concurrency();
	thread* threads = new thread[thread_count];

	for (int i = 0; i < thread_count; i++) {
		threads[i] = thread(random_fill_matrix_with_zero_padding,
			matrix + i * (height / thread_count), height / thread_count, width, padding_width, padding_height);
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

	int padding_width = 4 - filter_width % 4; // pad to make filter multiple of 4
	int padding_height = 4 - filter_height % 4; // pad to make filter multiple of 4

	double** filter = nullptr;
	double** source = nullptr;
	double** dest = nullptr;

	creat_matrix(filter, filter_height + padding_height, filter_width + padding_width);
	random_fill_matrix_with_zero_padding(filter, filter_height, filter_width, padding_width, padding_height);
	creat_matrix(source, source_height + padding_height, source_width + padding_width);
	parallel_random_fill_matrix_with_zero_padding(source, source_height, source_width, padding_width, padding_height);
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