#ifndef KPP_SYNC_SPINLOCK_HPP
#define KPP_SYNC_SPINLOCK_HPP

#include <linux/spinlock.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class SpinLock
    {
    private:
        spinlock_t lock_;

    public:
        SpinLock()
        {
            spin_lock_init(&lock_);
        }

        ~SpinLock() = default;

        SpinLock(const SpinLock&) = delete;
        SpinLock& operator=(const SpinLock&) = delete;
        SpinLock(SpinLock&&) = delete;
        SpinLock& operator=(SpinLock&&) = delete;

        void lock()
        {
            spin_lock(&lock_);
        }

        void unlock()
        {
            spin_unlock(&lock_);
        }
    };

} 

#endif // KPP_SYNC_SPINLOCK_HPP