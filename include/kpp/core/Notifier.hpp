#ifndef KPP_CORE_NOTIFIER_HPP
#define KPP_CORE_NOTIFIER_HPP

#include <linux/notifier.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    template <typename Owner>
    class Notifier
    {
    public:
        using NotifierFunc = int (Owner::*)(struct notifier_block*, unsigned long, void*);
        using RegisterFunc = int (*)(struct notifier_block*);
        using UnregisterFunc = int (*)(struct notifier_block*);

    private:
        struct notifier_block nb_;
        Owner* owner_;
        NotifierFunc callback_ptr_;
        UnregisterFunc unregister_ptr_;

        static int notifier_trampoline(struct notifier_block *nb, unsigned long action, void *data)
        {
            Notifier* self = container_of(nb, Notifier, nb_);
            if (self && self->owner_ && self->callback_ptr_) {
                return (self->owner_->*(self->callback_ptr_))(nb, action, data);
            }
            return NOTIFY_DONE;
        }

    public:
        Notifier(Owner* owner, NotifierFunc callback, RegisterFunc reg, UnregisterFunc unreg)
            : owner_(owner), callback_ptr_(callback), unregister_ptr_(unreg)
        {
            nb_.notifier_call = notifier_trampoline;
            reg(&nb_);
        }

        ~Notifier()
        {
            if (unregister_ptr_)
            {
                unregister_ptr_(&nb_);
            }
        }

        Notifier(const Notifier&) = delete;
        Notifier& operator=(const Notifier&) = delete;
        Notifier(Notifier&&) = delete;
        Notifier& operator=(Notifier&&) = delete;
    };
}

#endif // KPP_CORE_NOTIFIER_HPP