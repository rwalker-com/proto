#include "BufBound.h"

#include <stdio.h>
#include <sys/time.h>

typedef struct timeval timeval;

struct time : public timeval
{
    operator uint64_t() const { return (tv_sec * 1000000) + tv_usec; }
    operator timeval *() { return this; }
    uint64_t operator-(struct time & other) { return (uint64_t) * this - (uint64_t) other; }
};

int main(void)
{
    uint8_t buf[1000];
    uint8_t expect[1000];
    memset(expect, 1, 1000);
    chip::BufBound bb(buf, 1000);
    uint8_t c = 1;

    {
        struct time start;
        gettimeofday(&start, nullptr);

        for (int i = 0; i < 1000000; i++)
        {
            bb.Put(c);
        }
        struct time end;
        gettimeofday(&end, nullptr);

        printf("Put(c) took %llu\n", end - start);
        if (memcmp(buf, expect, 1000))
        {
            printf("fail!\n");
        }
    }

    {
        struct time start;
        gettimeofday(&start, nullptr);

        for (int i = 0; i < 1000000; i++)
        {
            bb.Put(&c, 1);
        }

        struct time end;
        gettimeofday(&end, nullptr);

        printf("Put(&c, 1) took %llu\n", end - start);
        if (memcmp(buf, expect, 1000))
        {
            printf("fail!\n");
        }
    }
}
