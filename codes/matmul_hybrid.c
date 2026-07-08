#include <stdio.h>
#include <stdlib.h>
#include <omp.h>  // OpenMP for parallelization
// Function to perform matrix multiplication (C = A * B)
void matmul_blocked(float *restrict A, float *restrict B, float *restrict C, int N, int block_size) {
    #pragma omp parallel for collapse(2) // Parallelize over blocks
    for (int i = 0; i < N; i += block_size) {
        for (int j = 0; j < N; j += block_size) {
            for (int k = 0; k < N; k += block_size) {
                // Iterate over sub-matrices (blocks) and perform multiplication
                for (int ii = i; ii < i + block_size && ii < N; ii+=4) {
                    for (int jj = j; jj < j + block_size && jj < N; jj+=4) {
                        //float acc0 = 0.0f, acc1 = 0.0f, acc2 = 0.0f, acc3 = 0.0f;
                        
                        int p=ii*N+jj;
                        float x00=C[p],x01=C[p+1],x02=C[p+2],x03=C[p+3];
                        p+=N;
                        float x10=C[p],x11=C[p+1],x12=C[p+2],x13=C[p+3];
                        p+=N;
                        float x20=C[p],x21=C[p+1],x22=C[p+2],x23=C[p+3];
                        p+=N;
                        float x30=C[p],x31=C[p+1],x32=C[p+2],x33=C[p+3];
                        for (int kk = k; kk < k + block_size && kk < N; ++kk) {
                            // C[ii * N + jj] += A[ii * N + kk] * B[kk * N + jj];
                            p=ii*N+kk;
                            float y0=A[p];
                            float y1=A[p+N];
                            float y2=A[p+2*N];
                            float y3=A[p+3*N];

                            p=kk*N+jj;
                            float z0=B[p];
                            float z1=B[p+N];
                            float z2=B[p+2*N];
                            float z3=B[p+3*N];

                            x00 += y0 * z0, x01 += y0 * z1, x02 += y0 * z2, x03 += y0 * z3;                            x00 += y0 * z0, x01+=y0*z1, x02+=y0*z2, x03+=y0*z3;
                            x10 += y1 * z0, x11 += y1 * z1, x12 += y1 * z2, x13 += y1 * z3;
                            x20 += y2 * z0, x21 += y2 * z1, x22 += y2 * z2, x23 += y2 * z3;
                            x30 += y3 * z0, x31 += y3 * z1, x32 += y3 * z2, x33 += y3 * z3;
                        }
                        //C[ii*N + jj] = acc0 + acc1 + acc2 + acc3;
                        p=ii*N+jj;
                        C[p]=x00,C[p+1]=x01,C[p+2]=x02,C[p+3]=x03;
                        p+=N;
                        C[p]=x10,C[p+1]=x11,C[p+2]=x12,C[p+3]=x13;
                        p+=N;
                        C[p]=x20,C[p+1]=x21,C[p+2]=x22,C[p+3]=x23;
                        p+=N;
                        C[p]=x30,C[p+1]=x31,C[p+2]=x32,C[p+3]=x33;
                    }
                }
            }
        }
    }
}

// Function to allocate a matrix and initialize with random values
float* create_matrix(int N) {
    float *mat = (float*)malloc(N * N * sizeof(float));
    for (int i = 0; i < N * N; i++) {
        mat[i] = (float)(rand() % 10000)/100.0 ; // Random values between 0 and 100
    }
    return mat;
}

// Function to print the matrix (for debugging)
void print_matrix(float *mat, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%f ", mat[i * N + j]);
        }
        printf("\n");
    }
}

signed main() {
    int N = 1024; // Size of the matrix (N x N)
    int block_size = 128; // Block size for blocking (tiled) matrix multiplication

    // Create matrices A, B, and C
    float *A = create_matrix(N);
    float *B = create_matrix(N);
    float *C = (float*)calloc(N * N, sizeof(float));  // Initialize C with zeros

    // Perform matrix multiplication: C = A * B
    matmul_blocked(A, B, C, N, block_size);

    // Optionally, print result (for small sizes of N)
    // print_matrix(C, N);

    // Clean up memory
    free(A);
    free(B);
    free(C);

    return 0;
}
