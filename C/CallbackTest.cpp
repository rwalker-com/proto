#include <stdio.h>

#include "Callback.h"

using namespace chip::Zcl;

class Resumer : public CallbackQueue
{
public:
    /**
     * @brief run this callback next Dispatch
     */
    void Resume(Callback<> * cb)
    {
        // always first thing: cancel to take ownership of
        //  cb members
        Enqueue(cb->Cancel());
    };

    void Dispatch()
    {
        CallbackInner ready = DequeueAll();
        // runs the ready list
        while (ready.mNext != &ready)
        {
            Callback<> * cb = Callback<>::FromInner(ready.mNext);
            cb->Run()(cb->mContext);
        }
    }
};

static void increment(int * v)
{
    (*v)++;
}

struct Resume
{
    Callback<> * cb;
    Resumer * resumer;
};

static void resume(struct Resume * me)
{
    me->resumer->Resume(me->cb);
}

static void canceler(CallbackInner * inner)
{
    inner->Cancel();
}

#include <stdio.h>
#define EXPECT(x)                                                                                                                  \
    do                                                                                                                             \
    {                                                                                                                              \
        if (!(x))                                                                                                                  \
        {                                                                                                                          \
            ++surprises;                                                                                                           \
            fprintf(stderr, "%s:%d: expected: \"%s\"\n", __FILE__, __LINE__, #x);                                                  \
        }                                                                                                                          \
    } while (0)

static int ResumerTest(void)
{
    int surprises = 0;

    int n = 1;
    Callback<> cb((void (*)(void *)) increment, &n);
    Callback<> cancelcb((void (*)(void *)) canceler, cb.Cancel());
    Resumer resumer;

    // Resume() works
    resumer.Resume(&cb);
    resumer.Dispatch();
    resumer.Resume(&cb);
    resumer.Dispatch();
    EXPECT(n == 3);

    n = 1;
    // Cancel cb before Dispatch() gets around to us (tests FIFO *and* cancel() from readylist)
    resumer.Resume(&cancelcb);
    resumer.Resume(&cb);
    resumer.Dispatch();
    EXPECT(n == 1);

    n = 1;
    // Resume() twice runs only once per
    resumer.Resume(&cb);
    resumer.Resume(&cb);
    resumer.Dispatch();
    resumer.Dispatch();
    EXPECT(n == 2);

    n = 1;
    // Resume() twice runs only once per
    resumer.Resume(&cb);
    resumer.Dispatch();
    resumer.Resume(&cb);
    resumer.Dispatch();
    EXPECT(n == 3);

    n = 1;
    // Resume() during Dispatch() runs only once, but does enqueue for next dispatch
    struct Resume res = { .cb = &cb, .resumer = &resumer };
    Callback<> resumecb((void (*)(void *)) resume, &res);
    resumer.Resume(&cb);
    resumer.Resume(&resumecb);
    resumer.Dispatch();
    EXPECT(n == 2);
    resumer.Dispatch();
    EXPECT(n == 3);

    return surprises;
}

class Notifier : public CallbackQueue
{
public:
    typedef void (*NotifyFn)(void *, int);

    /**
     * run all the callers
     */
    void Notify(int v)
    {
        CallbackInner ready = DequeueAll();
        while (ready.mNext != &ready)
        {
            //  Callbacks are one-shot, also removes item from ready
            Callback<NotifyFn> * cb = Callback<NotifyFn>::FromInner(ready.mNext);
            cb->Run()(cb->mContext, v);
        }
    }

    /**
     * @brief example
     */
    static void Cancel(CallbackInner * cb)
    {
        Dequeue(cb); // take off ready list
    }

    /**
     * @brief illustrate a case where this needs notification of cancellation
     */
    void Register(Callback<NotifyFn> * cb) { Enqueue(cb->Cancel(), Cancel); }
};

static void increment_by(int * n, int by)
{
    *n += by;
}

static int NotifierTest(void)
{
    int surprises = 0;

    int n = 1;
    Callback<Notifier::NotifyFn> cb((Notifier::NotifyFn) increment_by, &n);
    Callback<Notifier::NotifyFn> cancelcb((Notifier::NotifyFn) canceler, cb.Cancel());

    // safe to call anytime
    cb.Cancel();

    Notifier notifier;

    // Simple stuff works, e.g. persistent registration
    notifier.Register(&cb);
    notifier.Notify(1);
    notifier.Register(&cb);
    notifier.Notify(8);
    EXPECT(n == 10);

    n = 1;
    // Cancel cb before Dispatch() gets around to us (tests FIFO *and* cancel() from readylist)
    notifier.Register(&cancelcb);
    notifier.Register(&cb);
    notifier.Notify(8);
    EXPECT(n == 1);

    return surprises;
}

int main(void)
{
    return ResumerTest() + NotifierTest();
}
