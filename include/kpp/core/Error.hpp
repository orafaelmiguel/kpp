#ifndef KPP_CORE_ERROR_HPP
#define KPP_CORE_ERROR_HPP

#include <kpp/core/Freestanding.hpp>
#include <linux/errno.h>

namespace kpp
{
    class Error
    {
    private:
        int code_;

    public:
        explicit Error(int code) : code_(code) {}

        [[nodiscard]] int value() const noexcept
        {
            return code_;
        }

        [[nodiscard]] bool is_error() const noexcept
        {
            return code_ < 0;
        }
    };

    inline bool operator==(const Error& lhs, int rhs) noexcept
    {
        return lhs.value() == rhs;
    }

    inline bool operator!=(const Error& lhs, int rhs) noexcept
    {
        return lhs.value() != rhs;
    }

    inline bool operator==(const Error& lhs, const Error& rhs) noexcept
    {
        return lhs.value() == rhs.value();
    }

    inline bool operator!=(const Error& lhs, const Error& rhs) noexcept
    {
        return lhs.value() != rhs.value();
    }

} 

#endif 