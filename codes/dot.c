#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long int dot_fast(const  long long int * __restrict a,
               const long long int * __restrict b,
               int n)
{
    long long int s0 = 0.0f;
    long long int s1 = 0.0f;
    long long int s2 = 0.0f;
    long long int s3 = 0.0f;

    int i = 0;
    for (; i < n; i += 16) {
        s0 += a[i+0] * b[i+0];
        s0 += a[i+1] * b[i+1];
        s0 += a[i+2] * b[i+2];
        s0 += a[i+3] * b[i+3];

        s1 += a[i+4] * b[i+4];
        s1 += a[i+5] * b[i+5];
        s1 += a[i+6] * b[i+6];
        s1 += a[i+7] * b[i+7];

        s2 += a[i+8] * b[i+8];
        s2 += a[i+9] * b[i+9];
        s2 += a[i+10]* b[i+10];
        s2 += a[i+11]* b[i+11];

        s3 += a[i+12]* b[i+12];
        s3 += a[i+13]* b[i+13];
        s3 += a[i+14]* b[i+14];
        s3 += a[i+15]* b[i+15];
    }

    long long int sum = s0 + s1 + s2 + s3;

    return sum;
}

double now()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main()
{
    const int N = 1<<18; 

    long long int *a = aligned_alloc(64, N*sizeof(long long int));
    long long int *b = aligned_alloc(64, N*sizeof(long long int));

    for(int i=0;i<N;i++){
        a[i] = (rand()%10000)/100;
        b[i] = (rand()%10000)/100;
    }
    double t = now();
    // Warm up (loads data into cache)
    long long int cold = dot_fast(a,b,N);

    // Measure cached run
    double t0 = now();
    long long int r = dot_fast(a,b,N);
    double t1 = now();

    printf("warmup = %lld\n", cold);
    printf("result = %lld\n", r);
    printf("cached time = %.6f microseconds\n", (t1-t0)*1000000.0);
    printf("warmup time = %.6f microseconds\n", (t0-t)*1000000.0);

    free(a);
    free(b);
}