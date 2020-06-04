#include <string>
#include <stdio.h>

size_t fill(char * buf, size_t buflen)
{
    static const size_t needed = sizeof("hi there");
    memmove(buf, "hi there", std::min(buflen, needed));
    return needed;
}

int main(void)
{
    size_t to_fill = fill(NULL, 0);
    std::string out(to_fill, '\0');

    fill(&out.front(), to_fill);

    printf("got \"%s\"\n", out.c_str());
    return 0;
}
