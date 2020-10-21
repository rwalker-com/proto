#include <stdio.h>
#include <string.h>

int main()
{
    char * p;

    p = strndup("hi", 0);
    printf("p = %p\n", p);
    if (p)
    {
        printf("strlen(p) = %lu\n", strlen(p));
    }

    p = strndup((const char *) 0, 0);

    printf("p = %p\n", p);
    if (p)
    {
        printf("strlen(p) = %lu\n", strlen(p));
    }

    p = strndup((const char *) 0, 1);

    printf("p = %p\n", p);
    if (p)
    {
        printf("strlen(p) = %lu\n", strlen(p));
    }
}
