#include <stdio.h>
#include <stdlib.h>

#define n 10000
static int data[n];


int main() {
    srand(42);

    for(int i=0;i<n;i++) {
        data[i]=rand()%10000;
    }
    int max1=0;
    for(int i=0;i<n;i+=4) {
        max1=(max1>data[i])?max1:data[i];
        max1=(max1>data[i+1])?max1:data[i+1];
        max1=(max1>data[i+2])?max1:data[i+2];
        max1=(max1>data[i+3])?max1:data[i+3];
    }
    printf("%d\n",max1);
    return 0;
}
