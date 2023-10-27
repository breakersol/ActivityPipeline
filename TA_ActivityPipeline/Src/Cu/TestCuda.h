#ifndef TESTCUDA_H
#define TESTCUDA_H

#include "TA_ActivityPipeline_global.h"

// Thread block size
#define BLOCK_SIZE 16

// Matrices are stored in row-major order:
// M(row, col) = *(M.elements + row * M.stride + col)
struct Matrix
{
    int width;
    int height;
    int stride;
    float *elements = nullptr;
};

void ASYNC_PIPELINE_EXPORT MatMul(const Matrix A, const Matrix B, Matrix C);

void test(const Matrix A, const Matrix B, Matrix C)
{
    MatMul(A, B, C);
}

#endif // TESTCUDA_H
