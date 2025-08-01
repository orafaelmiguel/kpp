#ifndef KPP_ASYNC_WORKQUEUE_HPP
#define KPP_ASYNC_WORKQUEUE_HPP

#include <linux/workqueue.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/memory/UniquePtr.hpp>

namespace kpp
{
    template <typename F>
    struct Work
    {
        struct work_struct work;
        F func;

        template <typename... Args>
        Work(Args&&... args) : func(static_cast<Args&&>(args)...) {}

        static void work_callback(struct work_struct* w)
        {
            Work<F>* self = container_of(w, Work<F>, work);
            self->func();
            kpp::kfree(self);
        }
    };

    template <typename F>
    void schedule_work(F&& f)
    {
        auto work_item = kpp::make_unique<Work<F>>(GFP_ATOMIC, static_cast<F&&>(f));
        if (work_item)
        {
            INIT_WORK(&work_item->work, Work<F>::work_callback);
            schedule_work(&work_item.release()->work);
        }
    }

} 

#endif // KPP_ASYNC_WORKQUEUE_HPP