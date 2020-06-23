#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    uint8_t five = 5;
    uint8_t six  = 6;
    uint8_t diff = five - six;
    int ndiff    = diff;
    printf("five - six = %d\n"
           "abs(five - six) = %d\n"
           "diff = %d\n"
           "ndiff = %d",
           five - six, abs(five - six), diff, ndiff);
}
