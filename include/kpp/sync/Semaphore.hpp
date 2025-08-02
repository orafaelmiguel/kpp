#ifndef KPP_SYNC_SEMAPHORE_HPP
#define KPP_SYNC_SEMAPHORE_HPP

#include <linux/semaphore.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Semaphore
    {
    private:
        struct semaphore sem_;

    public:
        explicit Semaphore(int count)
        {
            sema_init(&sem_, count);
        }

        ~Semaphore() = default;

        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

        void down()
        {
            down(&sem_);
        }

        int down_interruptible()
        {
            return down_interruptible(&sem_);
        }

        int down_killable()
        {
            return down_killable(&sem_);
        }

        int down_timeout(long timeout_jiffies)
        {
            return down_timeout(&sem_, timeout_jiffies);
        }

        void up()
        {
            up(&sem_);
        }
    };

} 

#endif // KPP_SYNC_SEMAPHORE_HPP