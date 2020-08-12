#include "BufBound.h"

void Put()
{
    chip::BufBound bb(nullptr, 0);
    uint8_t c = 1;
    bb.Put(c);
    bb.Put(&c, 1);
}
