#ifndef KPP_SYNC_SCOPEDLOCK_HPP
#define KPP_SYNC_SCOPEDLOCK_HPP

#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    template <typename Lockable>
    class ScopedLock
    {
    private:
        Lockable& lock_;

    public:
        explicit ScopedLock(Lockable& lock) : lock_(lock)
        {
            lock_.lock();
        }

        ~ScopedLock()
        {
            lock_.unlock();
        }

        ScopedLock(const ScopedLock&) = delete;
        ScopedLock& operator=(const ScopedLock&) = delete;
        ScopedLock(ScopedLock&&) = delete;
        ScopedLock& operator=(ScopedLock&&) = delete;
    };

} 

#endif // KPP_SYNC_SCOPEDLOCK_HPP