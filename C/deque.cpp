#include "Deque.h"

#include <stdio.h>

using namespace chip;
class Letter : public Deque
{
public:
    char mC;
    Letter(char c) : mC(c){};
};

int main(void)
{
    Letter a('a');
    Letter b('b');
    Letter c('c');
    Letter d('d');

    a.InsertAfter(&b);

    a.While([&a](Deque * item) -> bool {
        Letter * letter = static_cast<Letter *>(item);

        printf("%c", letter->mC);
        if (letter->Next() == static_cast<Deque *>(&a))
        {
            printf("\n");
            return false;
        }
        else
        {
            return true;
        }
    });
}
