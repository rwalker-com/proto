#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ASSERT(x) do { if (!(x)) { fprintf(stderr,"assertion failed: %s\n", #x); exit(1); } } while (0)

#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

/* #define DEBUG(...) 0 */

int main()
{
    char abuf[256],bbuf[256];

    while (2 == fscanf(stdin, "%s %s", abuf, bbuf)) {
        int   diff = 0;
        int   alen = strlen(abuf);
        int   blen = strlen(bbuf);

        diff = alen - blen;

        DEBUG("comparing \"%s\" vs \"%s\"\n", abuf, bbuf);

        if (diff == 0) {
            char* a = abuf;
            char* b = bbuf;
            DEBUG("same len\n");
            for (a = abuf, b=bbuf;
                 diff <= 1 && *a;
                 a++, b++) {
                if (*a != *b) diff++;
            }
        } else if (diff == 1 || diff == -1) {
            char* pshort;
            char* plong;
            if (alen < blen) {
                pshort = abuf;
                plong  = bbuf;
            } else {
                pshort = bbuf;
                plong  = abuf;
            }
            DEBUG("\"%s\" is shorter\n", pshort);

            diff = 0;
            while (*pshort && diff <= 1) {
                if (*pshort++ != *plong++) {
                    diff++;
                    plong++;
                }
            }
        } else {
            diff = 2;
        }
        DEBUG("\"%s\" one away from \"%s\": %s\n", abuf, bbuf, diff <= 1 ? "true" : "false");
        fprintf(stdout, diff <= 1 ? "true\n" : "false\n");

    }

    return 0;
}

