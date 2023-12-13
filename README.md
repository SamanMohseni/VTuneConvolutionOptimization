# Improved Matrix Convolution Algorithm based on VTune Profiler feedback
This project is part of **Advanced Microprocessor** course.

This project presents an improved matrix convolution algorithm optimized using Intel VTune Profiler feedback. The algorithm focuses on optimizing data localization, reducing switching time between DSB and MITE, minimizing hazards, and enhancing branch prediction accuracy. Additionally, the algorithm leverages various instruction sets, including AVX, AVX2, and SSE2, to achieve vectorization and improve performance.

## Performance Results

The following table summarizes the performance results obtained after various optimization trials (Available in trials folder, tested on an Intel(R) Core(TM) i7-6700HQ CPU):

| Execution Time | Features | Version |
|----------------|----------|---------|
| 249s           | Single-threaded, no vectorization, no compiler optimization | V1 |
| 64s            | Added multi-threaded processing to `convolve` function | V2 |
| 52s            | Added multi-threaded processing to `create_random_matrix` function | V3 |
| 13.027s        | Post compiler-assisted vectorization, with O2 optimization enabled. Used SSE2 vector instructions. | V4 |
| 11.334s        | Post manual vectorization with AVX2 instructions. | V5 |
| 9.219s         | Combination of vectorization and scalar computations | V6 |
| 8.851s         | Localized memory access by computing two rows of the filter across all rows, obtaining partial sums, and repeating until the final sum is achieved | V7 |
| 8.854s         | Further localized memory access by computing one row of the filter across all rows. However, we had to forego the combination of vectorization and scalar computations. | V8 |
| 8.052s         | Performed part of the V8 computations in scalar. | V9 |
