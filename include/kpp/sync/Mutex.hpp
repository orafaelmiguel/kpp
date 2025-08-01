#ifndef KPP_SYNC_MUTEX_HPP
#define KPP_SYNC_MUTEX_HPP

#include <linux/mutex.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Mutex
    {
    private:
        struct mutex lock_;

    public:
        Mutex()
        {
            mutex_init(&lock_);
        }

        ~Mutex()
        {
            mutex_destroy(&lock_);
        }

        Mutex(const Mutex&) = delete;
        Mutex& operator=(const Mutex&) = delete;
        Mutex(Mutex&&) = delete;
        Mutex& operator=(Mutex&&) = delete;

        void lock()
        {
            mutex_lock(&lock_);
        }

        void unlock()
        {
            mutex_unlock(&lock_);
        }
    };

} 

#endif // KPP_SYNC_MUTEX_HPP