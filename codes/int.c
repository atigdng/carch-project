#include <stdio.h>
#include <stdlib.h>

#define n 10000
static int data[n];

int main() {
    srand(42);

    for(int i=0;i<n;i++) {
        data[i]=rand()%10000;
    }
    float max2=0.0f;
    int max1=0;
    /*for(int i=0;i<n;i+=4) {
        max2=(max2>(float)data[i])?max2:(float)data[i];
        max2=(max2>(float)data[i+1])?max2:(float)data[i+1];
        max2=(max2>(float)data[i+2])?max2:(float)data[i+2];
        max1=(max1>data[i+3])?max1:data[i+3];
    }*/
    printf("%d\n",((int)max2>max1)?(int)max2:max1);
}