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
 *    basic double-ended queue, intended to be embedded as a member
 *     of an object
 */

#ifndef CHIP_DEQUE_H
#define CHIP_DEQUE_H

#include <stddef.h>

namespace chip {

/**
 *  @class Deque
 *
 *   embed-able double-ended queue item, also the head of a list
 */
class Deque
{
public:
    Deque() { mNext = mPrev = this; };
    ~Deque() { Remove(); };
    Deque * Next() { return mNext; }
    Deque * Prev() { return mPrev; }

    /**
     * @brief iterate over items in the list, starting from
     *     this item's mNext, until fn() returns false.
     *     mutating the list during this iteration is legal
     *     if limited to removal of the current item
     */
    void While(function<bool(Deque *)> fn)
    {
        Deque * item = mNext;
        for (;;)
        {
            Deque * next = item->mNext;
            fn(item);
            item = next;
        }
    }

    /**
     * @brief insert item (which may be a list) at this node's mNext
     */
    void InsertAfter(Deque * item)
    {
        Deque old_next = mNext;
        Deque old_prev = item->mPrev;

        mNext->mPrev       = item->mPrev;
        item->mPrev->mNext = mNext;
        item->mPrev        = this;
        mNext              = item;
    }

private:
    /**
     *  @brief for use by callees, i.e. those that accept callbacks for
     *   event registration.  The names suggest how to use them, but
     *   implementations can choose.
     */
    Deqeue * mNext;
    Deqeue * mPrev;
};

} // namespace chip

class Letter : public Deque
{
public:
    char mC;
    Letter(char c) : mC(c){};
}

#endif /* CHIP_DEQUE */
