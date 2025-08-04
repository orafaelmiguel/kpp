#ifndef KPP_NET_DSTENTRY_HPP
#define KPP_NET_DSTENTRY_HPP

#include <net/dst.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class DstEntry
    {
    private:
        struct dst_entry* dst_;

    public:
        explicit DstEntry(struct dst_entry* dst) : dst_(dst) {}

        ~DstEntry()
        {
            if (dst_)
            {
                dst_release(dst_);
            }
        }

        DstEntry(const DstEntry&) = delete;
        DstEntry& operator=(const DstEntry&) = delete;

        DstEntry(DstEntry&& other) noexcept : dst_(other.dst_)
        {
            other.dst_ = nullptr;
        }

        DstEntry& operator=(DstEntry&& other) noexcept
        {
            if (this != &other)
            {
                if (dst_)
                {
                    dst_release(dst_);
                }
                dst_ = other.dst_;
                other.dst_ = nullptr;
            }
            return *this;
        }

        struct dst_entry* native_handle() const
        {
            return dst_;
        }

        struct dst_entry* operator->() const
        {
            return dst_;
        }

        explicit operator bool() const noexcept
        {
            return dst_ != nullptr;
        }
    };

} 

#endif // KPP_NET_DSTENTRY_HPP