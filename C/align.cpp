#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint8_t bytes[9] = { 0 };

    *(uint64_t *) (bytes + 1) = 0x0102030405060708ull;

    printf("0x%llx\n", *(uint64_t *) (bytes + 1));
    printf("0x%llx\n", *(uint64_t *) bytes);
}
