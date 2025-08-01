#ifndef KPP_ASYNC_TIMER_HPP
#define KPP_ASYNC_TIMER_HPP

#include <linux/timer.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Timer
    {
    private:
        struct timer_list timer_;

        template<typename F>
        static void timer_callback(struct timer_list* t)
        {
            F* func = from_timer(func, t, timer_);
            (*func)();
        }

    public:
        Timer() = default;

        ~Timer()
        {
            del_timer_sync(&timer_);
        }

        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;
        Timer(Timer&&) = delete;
        Timer& operator=(Timer&&) = delete;

        template<typename F>
        void setup(F* func)
        {
            timer_setup(&timer_, timer_callback<F>, 0);
        }

        void schedule(unsigned long expires_jiffies)
        {
            mod_timer(&timer_, jiffies + expires_jiffies);
        }

        void cancel()
        {
            del_timer_sync(&timer_);
        }
    };

} 

#endif // KPP_ASYNC_TIMER_HPP