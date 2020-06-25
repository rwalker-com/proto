/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *  @file
 *    This file contains definitions for Callback objects for registering with
 *     Clusters and the Device
 */

#ifndef CHIP_ZCL_CALLBACK_H_
#define CHIP_ZCL_CALLBACK_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @namespace chip::Zcl
 *
 * Definitions for working with CHIP's Zigbee Cluster
 *        Library implementation
 */
namespace chip {
namespace Zcl {

/**
 *  @class CallbackInner
 *
 *    "Private" members of a Callback used by subsystems while a callback is
 *     registered with them.
 *
 */
class CallbackInner
{
public:
    /**
     *  @brief for use by callees, i.e. those that accept callbacks for
     *   event registration.  The names suggest how to use them, but
     *   implementations can choose.
     */
    CallbackInner * mNext;
    CallbackInner * mPrev;

    void * mInfoPtr;
    uint64_t mInfoScalar;

    /**
     * when non-null, indicates the Callback is registered with
     *        a subsystem and that Inner members belong to
     *        that subsystem
     */
    void (*mCancel)(CallbackInner *);

    CallbackInner()
    {
        mNext = mPrev = this;
        mCancel       = NULL;
    };

    CallbackInner * Cancel()
    {
        if (mCancel != NULL)
        {
            void (*cancel)(CallbackInner *) = mCancel;
            mCancel                         = 0;
            cancel(this);
        }
        return this;
    }
};

/**
 *  @class Callback
 *
 *    Base struct used for registration of items of interest, includes
 *     memory for list management and storing information about the registration's
 *     meaning.  Callback also defines cancellation.
 *    Callbacks can be registered with exactly one callee at a time. While
 *     registered (as indicated by a non-null mCancel function), all fields of
 *     the Callback save usercontext are "owned" by the callee, and should not
 *     be touched unless Cancel() has first been called.
 *    When a callee accepts a Callback for registration, step one is always Cancel(),
 *     in order to take ownership of next, prev, info_ptr, and info_scalar.
 *    Since this class does not define the notification function prototype, its intended
 *     use is as a base class for callback functions of various signatures.
 *
 */
template <class T = void (*)(void *)>
class Callback : private CallbackInner
{
public:
    /**
     *  owned by the caller, the caller's user context
     */
    void * mContext;

    /**
     * where to call when the event of interest has occured
     */
    T mRun;

    /**
     * run enforces one-shot semantics
     */
    T Run()
    {
        Cancel();
        return mRun;
    }

    bool IsRegistered() { return (mCancel != NULL); }

    /**
     * Cancel, i.e. de-register interest in the event,
     *  this is the only way to get access to the Inner
     */
    CallbackInner * Cancel() { return CallbackInner::Cancel(); };

    /**
     * public constructor
     */
    Callback(T run, void * context) : mRun(run), mContext(context) { CallbackInner(); };

    /**
     * TODO: type-safety?
     */
    static Callback * FromInner(CallbackInner * inner) { return static_cast<Callback *>(inner); }
};

class CallbackQueue
{
public:
    /**
     * @brief run this callback next Notify
     */
    void Enqueue(CallbackInner * inner)
    {
        // add to a doubly-linked list, set cancel function
        inner->mPrev       = mHead.mPrev;
        mHead.mPrev->mNext = inner;
        mHead.mPrev        = inner;
        inner->mNext       = &mHead;
        inner->mCancel     = Dequeue;
    };

    CallbackQueue() : mHead() { mHead.mNext = mHead.mPrev = &mHead; };

    /**
     * Dequeue all, return in a stub
     */
    CallbackInner DequeueAll()
    {
        CallbackInner ready;

        if (mHead.mNext != &mHead)
        {
            ready.mNext        = mHead.mNext;
            ready.mPrev        = mHead.mPrev;
            ready.mPrev->mNext = &ready;
            ready.mNext->mPrev = &ready;

            mHead.mNext = mHead.mPrev = &mHead;
        }
        return ready;
    }

    static void Dequeue(CallbackInner * inner)
    {
        inner->mNext->mPrev = inner->mPrev;
        inner->mPrev->mNext = inner->mNext;
        inner->mNext = inner->mPrev = inner;
        inner->mCancel              = NULL;
    }

private:
    CallbackInner mHead;
};

} // namespace Zcl
} // namespace chip

#endif /* CHIP_ZCL_CALLBACK_H_ */
