#ifndef MATMUL_H
#define MATMUL_H

#include <algorithm>

void matmul_ref(const int* const matrixA, const int* const matrixB,
                int* const matrixC, const int n);
void matmul_optimized(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n);

#endif