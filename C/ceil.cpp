#include <math.h>
#include <stdio.h>

int main(void)
{
    size_t bitsLeftToRead = 100;
    size_t tlvBytesLength = ceil(bitsLeftToRead / 8);
    printf("tlvBytesLength = %lu\n", tlvBytesLength);
    return 0;
}
