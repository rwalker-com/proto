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
        mCancel       = nullptr;
    };

    CallbackInner * Cancel()
    {
        if (mCancel != nullptr)
        {
            void (*cancel)(CallbackInner *) = mCancel;
            mCancel                         = nullptr;
            cancel(this);
        }
        return this;
    }
    ~CallbackInner() { Cancel(); };
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
 *     in order to take ownership of Inner members next, prev, info_ptr, and info_scalar.
 *    This template class also defines a default notification function prototype.
 *
 *   One-shot semantics can be accomplished by calling Cancel() on
 *
 *
 */
template <class T = void (*)(void *)>
class Callback : private CallbackInner
{
public:
    /**
     * pointer to owner context, normally passed to the run function
     */
    void * mContext;

    /**
     * where to call when the event of interest has occured
     */
    T mCall;

    /**
     * Indication that the Callback is registered with a notifier
     */
    bool IsRegistered() { return (mCancel != nullptr); }

    /**
     * Cancel, i.e. de-register interest in the event,
     *  this is the only way to get access to the Inner, to enqueue,
     *  store any per-registration state
     */
    CallbackInner * Cancel() { return CallbackInner::Cancel(); };

    /**
     * public constructor
     */
    Callback(T call, void * context) : mCall(call), mContext(context) { CallbackInner(); };

    /**
     * TODO: type-safety?
     */
    static Callback * FromInner(CallbackInner * inner) { return static_cast<Callback *>(inner); }
};

/**
 * @brief core of a simple doubly-linked list Callback keeper-tracker-of
 *
 */
class CallbackDeque
{
public:
    /**
     * @brief appends
     */
    void Enqueue(CallbackInner * inner) { Enqueue(inner, Dequeue); };

    /**
     * @brief appends with overridden cancel function, in case the
     *   list change requires some other state update.
     */
    void Enqueue(CallbackInner * inner, void (*cancel)(CallbackInner *))
    {
        // add to a doubly-linked list, set cancel function
        inner->Cancel(); // make doubly-sure we're not corrupting another list somewhere
        inner->mPrev       = mHead.mPrev;
        mHead.mPrev->mNext = inner;
        mHead.mPrev        = inner;
        inner->mNext       = &mHead;
        inner->mCancel     = cancel;
    };

    CallbackDeque() : mHead() { mHead.mNext = mHead.mPrev = &mHead; };

    /**
     * Dequeue all, return in a stub. does not cancel the inners, as the list
     *   members are still in use
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
        inner->mCancel              = nullptr;
    }

    CallbackInner mHead;
};

} // namespace Zcl
} // namespace chip

#endif /* CHIP_ZCL_CALLBACK_H_ */
