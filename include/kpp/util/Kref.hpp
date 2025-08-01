#ifndef KPP_UTIL_KREF_HPP
#define KPP_UTIL_KREF_HPP

#include <linux/kref.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Kref
    {
    private:
        struct kref ref_;

    public:
        Kref()
        {
            kref_init(&ref_);
        }

        ~Kref() = default;

        Kref(const Kref&) = delete;
        Kref& operator=(const Kref&) = delete;
        Kref(Kref&&) = delete;
        Kref& operator=(Kref&&) = delete;

        void init()
        {
            kref_init(&ref_);
        }

        void get()
        {
            kref_get(&ref_);
        }

        [[nodiscard]] int put(void (*release)(struct kref *kref))
        {
            return kref_put(&ref_, release);
        }

        struct kref* native_handle()
        {
            return &ref_;
        }
    };

}

#endif // KPP_UTIL_KREF_HPP