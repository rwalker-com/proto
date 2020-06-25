#include "Callback.h"

using namespace chip::Zcl;

class Resumer
{
public:
    /**
     * @brief run this callback next Dispatch
     */
    void Resume(Callback<> * cb)
    {
        // always first thing: cancel to take ownership of
        //  cb members
        CallbackInner * inner = cb->Cancel();

        // add to a doubly-linked list
        inner->mPrev       = mHead.mPrev;
        mHead.mPrev->mNext = inner;
        mHead.mPrev        = inner;
        inner->mNext       = &mHead;
        inner->mCancel     = Resumer::Cancel;
    };

    Resumer() : mHead() { mHead.mNext = mHead.mPrev = &mHead; };

    void Dispatch()
    {
        // empty? we're done
        if (mHead.mNext == &mHead)
        {
            return;
        }

        // move everything to a ready list
        CallbackInner ready;

        ready.mNext        = mHead.mNext;
        ready.mPrev        = mHead.mPrev;
        ready.mPrev->mNext = &ready;
        ready.mNext->mPrev = &ready;

        mHead.mNext = mHead.mPrev = &mHead;

        // runs the ready list
        while (ready.mNext != &ready)
        {
            Callback<> * cb = Callback<>::FromInner(ready.mNext);
            cb->Run()(cb->mContext);
        }
    }

    static void Cancel(CallbackInner * inner)
    {
        inner->mNext->mPrev = inner->mPrev;
        inner->mPrev->mNext = inner->mNext;
        inner->mNext = inner->mPrev = inner;
        inner->mCancel              = NULL;
    }

private:
    CallbackInner mHead;
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
    Callback<> cancelcb((void (*)(void *)) Resumer::Cancel, &cb);
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
    static void Cancel(CallbackInner * cb) { Dequeue(cb); }

    // always first thing: cancel to take ownership of
    //  cb members, then enqueue
    void Register(Callback<NotifyFn> * cb) { Enqueue(cb->Cancel()); }
};

static void increment_by(int * n, int by)
{
    *n += by;
}

static void canceler(CallbackInner * inner)
{
    inner->Cancel();
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

    // Simple stuff works
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
