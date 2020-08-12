#include "Deque.h"

#include <stdio.h>

#include <string>

using namespace chip;

// type using Deque as a base class, no head pointer
class Letters : public Deque<Letters>
{
public:
    char c;
    Letters(char c) : Deque(this), c(c){};

    std::string to_string()
    {
        std::string ret;
        Foreach([&](Deque * item) -> void { ret.push_back(static_cast<Letters *>(item)->c); });
        return ret;
    };

    std::string to_string_r()
    {
        std::string ret;
        auto lambda = [&](Deque * item) -> void { ret.push_back(static_cast<Letters *>(item)->c); };

        ForeachR([&](Deque * item) -> void { ret.push_back(static_cast<Letters *>(item)->c); });
        return ret;
    };
};

// type using Deque embedded, intended for lists using a head pointer
class Letter
{
public:
    char c;
    Letter(char c) : c(c), mDeque(this){};

    std::string to_string()
    {
        std::string ret;
        mDeque.Foreach([&](Letter * item) -> void { ret.push_back(item->c); });
        return ret;
    };

    std::string to_string_r()
    {
        std::string ret;
        mDeque.ForeachR([&](Letter * item) -> void { ret.push_back(item->c); });
        return ret;
    };

    void Insert(Letter * item) { mDeque.Insert(&item->mDeque); }

    Deque<Letter> mDeque;
};

#define EXPECT(x)                                                                                                                  \
    do                                                                                                                             \
    {                                                                                                                              \
        if (!(x))                                                                                                                  \
        {                                                                                                                          \
            fprintf(stderr, "%s:%d: expected %s\n", __FILE__, __LINE__, #x);                                                       \
            ++surprises;                                                                                                           \
        }                                                                                                                          \
    } while (0)

int main(void)
{
    int surprises = 0;

    // with no heads

    {
        Letters a('a');

        EXPECT(a.to_string() == "a");

        Letters b('b');
        Letters c('c');
        Letters d('d');

        EXPECT(a.to_string_r() == "a");

        a.Insert(&b);
        EXPECT(a.to_string() == "ab");
        EXPECT(a.to_string_r() == "ba");

        b.Remove();
        EXPECT(a.to_string() == "a");

        b.Insert(&a);
        EXPECT(a.to_string() == "ab");
        EXPECT(b.to_string() == "ba");

        c.Insert(&d);
        EXPECT(c.to_string() == "cd");

        a.Insert(&c);
        EXPECT(a.to_string() == "abcd");
        EXPECT(c.to_string() == "cdab");
    }

    {
        Deque<Letter> ab(nullptr);
        Letter a('a');
        Letter b('b');

        EXPECT(a.to_string() == "a");
        a.Insert(&b);
        EXPECT(a.to_string() == "ab");

        ab.Insert(&a.mDeque);
        EXPECT(a.to_string() == "ab");
        EXPECT(b.to_string() == "ba");

        Deque<Letter> cd(nullptr);

        Letter c('c');
        Letter d('d');
        //        c.Insert(&d);
        // EXPECT(c.to_string() == "cd");

        cd.Insert(&c.mDeque);
        EXPECT(c.to_string() == "c");

        ab.Insert(&cd);
        EXPECT(a.to_string() == "abc");

        c.Insert(&d);
        EXPECT(a.to_string() == "abdc");
    }

    return surprises;
}
