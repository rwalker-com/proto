#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define STATIC_ASSERT(name, x) static __inline void static_assert_##name(void) { switch (0) { case (x): case 0/*assertion failed*/: ; }}
//STATIC_ASSERT(0)

const size_t kSetupCodeLengthInBits = 32;
const size_t kManualSetupShortCodeLengthInChars = 10;

STATIC_ASSERT(x, (1L<<32)-1 <= 1e10L)

//STATIC_ASSERT(y, (1L<<32)-1 <= 1e9L)


#include <math.h>
int main(void) {

    printf("%lld\n", (1LL<<32)-1);

    printf("%f\n", ceil(log10(pow(2, 33))));
}
