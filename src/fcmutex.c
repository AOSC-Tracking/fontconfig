/*
 * fontconfig/src/fcmutex.c
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the author(s) not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE AUTHOR(S) DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "fcint.h"

void FcMutexTryInitThenLock(FcMutex **m)
{
    FcMutex *lock;
    do {
        lock = fc_atomic_ptr_get (m);
        if(lock == NULL) {
            lock = malloc(sizeof(FcMutex));
            fc_mutex_impl_init(lock);
            if(!fc_atomic_ptr_cmpexch(m, NULL, lock)){
                fc_mutex_impl_finish(lock);
                free(lock);
                lock = NULL;
            }
        }
    } while (lock == NULL);
    fc_mutex_impl_lock(lock);
}

void FcMutexUnlock(FcMutex** m)
{
    FcMutex *lock;
    lock = fc_atomic_ptr_get(m);
    fc_mutex_impl_unlock(lock);
}

void FcMutexFinish(FcMutex** m)
{
    FcMutex *lock;
    lock = fc_atomic_ptr_get(m);
    if (lock && fc_atomic_ptr_cmpexch(m, lock, NULL)){
        fc_mutex_impl_finish(lock);
        free(lock);
    }
}
