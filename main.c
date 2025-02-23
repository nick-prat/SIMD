#include "stdio.h"

extern int multiply(int, int);

int main(int argc, char** argv) {
    printf("Hello World %d!\n", multiply(10, 500));
    return 0;
}