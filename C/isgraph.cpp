#include <ctype.h>
#include <stdio.h>

int main()
{
    int cols = 0;
    for (int i = 0; i < 0x7fffffff; i++)
    {
        if (isgraph(i))
        {
            printf("%c ", (char) i);
            cols += 1;
            cols &= 15;
            if (!cols)
            {
                printf("\n");
            }
        }
    }
    printf("\ndone\n");
}
