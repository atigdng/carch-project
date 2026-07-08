#include <stdio.h>
#include <stdlib.h>
#include <omp.h>  // OpenMP for parallelization
// Function to perform matrix multiplication (C = A * B)
void matmul_blocked(int *restrict A, int *restrict B, int *restrict C, int N, int block_size) {
    #pragma omp parallel for collapse(2) // Parallelize over blocks
    for (int i = 0; i < N; i += block_size) {
        for (int j = 0; j < N; j += block_size) {
            for (int k = 0; k < N; k += block_size) {
                // Iterate over sub-matrices (blocks) and perform multiplication
                for (int ii = i; ii < i + block_size && ii < N; ii+=4) {
                    for (int jj = j; jj < j + block_size && jj < N; jj+=4) {
                        int p=ii*N+jj;
                        int x00=C[p],x01=C[p+1],x02=C[p+2],x03=C[p+3];
                        p+=N;
                        int x10=C[p],x11=C[p+1],x12=C[p+2],x13=C[p+3];
                        p+=N;
                        int x20=C[p],x21=C[p+1],x22=C[p+2],x23=C[p+3];
                        p+=N;
                        int x30=C[p],x31=C[p+1],x32=C[p+2],x33=C[p+3];
                        for (int kk = k; kk < k + block_size && kk < N; ++kk) {
                            p=ii*N+kk;
                            int y0=A[p];
                            int y1=A[p+N];
                            int y2=A[p+2*N];
                            int y3=A[p+3*N];

                            p=kk*N+jj;
                            int z0=B[p];
                            int z1=B[p+N];
                            int z2=B[p+2*N];
                            int z3=B[p+3*N];

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
int* create_matrix(int N) {
    int *mat = (int*)malloc(N * N * sizeof(int));
    for (int i = 0; i < N * N; i++) {
        mat[i] = (int)(rand() % 10000)/100 ; // Random values between 0 and 100
    }
    return mat;
}

// Function to print the matrix (for debugging)
void print_matrix(int *mat, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", mat[i * N + j]);
        }
        printf("\n");
    }
}

signed main() {
    int N = 1024; // Size of the matrix (N x N)
    int block_size = 128; // Block size for blocking (tiled) matrix multiplication

    // Create matrices A, B, and C
    int *A = create_matrix(N);
    int *B = create_matrix(N);
    int *C = (int*)calloc(N * N, sizeof(int));  // Initialize C with zeros

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