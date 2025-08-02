#ifndef KPP_CORE_KSET_HPP
#define KPP_CORE_KSET_HPP

#include <linux/kobject.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class KSet
    {
    private:
        struct kset* kset_;

    public:
        explicit KSet(const char* name, struct kobject* parent_kobj)
        {
            kset_ = kset_create_and_add(name, NULL, parent_kobj);
        }

        ~KSet()
        {
            if (kset_)
            {
                kset_unregister(kset_);
            }
        }

        KSet(const KSet&) = delete;
        KSet& operator=(const KSet&) = delete;

        KSet(KSet&& other) noexcept : kset_(other.kset_)
        {
            other.kset_ = nullptr;
        }

        KSet& operator=(KSet&& other) noexcept
        {
            if (this != &other)
            {
                if (kset_)
                {
                    kset_unregister(kset_);
                }
                kset_ = other.kset_;
                other.kset_ = nullptr;
            }
            return *this;
        }

        struct kset* native_handle() const
        {
            return kset_;
        }

        struct kobject* kobj() const
        {
            return kset_ ? &kset_->kobj : nullptr;
        }

        explicit operator bool() const noexcept
        {
            return kset_ != nullptr;
        }
    };

} 

#endif // KPP_CORE_KSET_HPP