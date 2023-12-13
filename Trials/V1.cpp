#include <iostream>

using namespace std;

void convolve(double** source, double** dest, int source_height, int source_width, double** filter, int filter_height, int filter_width) {
	for (int s_h = 0; s_h < source_height - (filter_height - 1); s_h++) {
		for (int s_w = 0; s_w < source_width - (filter_width - 1); s_w++) {
			double sum = 0;
			for (int f_h = 0; f_h < filter_height; f_h++) {
				for (int f_w = 0; f_w < filter_width; f_w++) {
					sum += source[s_h + f_h][s_w + f_w] * filter[f_h][f_w];
				}
			}
			dest[s_h][s_w] = sum;
		}
	}
}

void creat_random_matrix(double**& matrix, int height, int width) {
	matrix = new double* [height];
	for (int i = 0; i < height; i++) {
		matrix[i] = new double[width];
		for (int j = 0; j < width; j++) {
			matrix[i][j] = (rand() % 1000) / 1000.0;
		}
	}
}

void creat_matrix(double**& matrix, int height, int width) {
	matrix = new double* [height];
	for (int i = 0; i < height; i++) {
		matrix[i] = new double[width];
	}
}

int main() {
	const int source_height = 2048;
	const int source_width = 2048;
	const int filter_height = 15;
	const int filter_width = 15;

	double** filter = nullptr;
	double** source = nullptr;
	double** dest = nullptr;

	creat_random_matrix(filter, filter_height, filter_width);
	creat_random_matrix(source, source_height, source_width);
	creat_matrix(dest, source_height - filter_height + 1, source_width - filter_width + 1);

	cout << "Convolution Started..." << endl;

	convolve(source, dest, source_height, source_width, filter, filter_height, filter_width);

	cout << "Convolution complete" << endl;

	return 0;
}