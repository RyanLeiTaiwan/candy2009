#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "image.h"

int main()
{
    Matrix A;

    printf("Hello world!!\n");
    imread(  "redtest.bmp", &A );
    dump( &A, "A", BB, INT );
    return 0;
}
