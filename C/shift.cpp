#include <stdint.h>
#include <stdio.h>
int main(void)
{
    uint16_t x = 10;
    uint32_t y = 0;

    y |= x << 16;

    printf("0x%08x", y);
}
