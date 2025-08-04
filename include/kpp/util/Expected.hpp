#ifndef KPP_UTIL_EXPECTED_HPP
#define KPP_UTIL_EXPECTED_HPP

#include <kpp/core/Freestanding.hpp>
#include <kpp/core/Error.hpp>

namespace kpp
{
    template <typename T, typename E>
    class Expected
    {
    private:
        union {
            T value_;
            E error_;
        };
        bool has_value_;

    public:
        Expected(const T& val) : value_(val), has_value_(true) {}
        Expected(T&& val) : value_(static_cast<T&&>(val)), has_value_(true) {}
        Expected(const E& err) : error_(err), has_value_(false) {}
        Expected(E&& err) : error_(static_cast<E&&>(err)), has_value_(false) {}

        ~Expected()
        {
            if (has_value_) {
                value_.~T();
            } else {
                error_.~E();
            }
        }

        Expected(const Expected&) = delete;
        Expected& operator=(const Expected&) = delete;

        Expected(Expected&& other) noexcept : has_value_(other.has_value_)
        {
            if (has_value_) {
                new (&value_) T(static_cast<T&&>(other.value_));
            } else {
                new (&error_) E(static_cast<E&&>(other.error_));
            }
        }

        Expected& operator=(Expected&& other) noexcept
        {
            if (this != &other)
            {
                this->~Expected();
                new (this) Expected(static_cast<Expected&&>(other));
            }
            return *this;
        }

        [[nodiscard]] bool has_value() const noexcept
        {
            return has_value_;
        }

        explicit operator bool() const noexcept
        {
            return has_value_;
        }

        T& value() &
        {
            return value_;
        }

        const T& value() const &
        {
            return value_;
        }

        E& error() &
        {
            return error_;
        }

        const E& error() const &
        {
            return error_;
        }

        T* operator->()
        {
            return &value_;
        }

        const T* operator->() const
        {
            return &value_;
        }

        T& operator*() &
        {
            return value_;
        }

        const T& operator*() const &
        {
            return value_;
        }
    };
}

#endif // KPP_UTIL_EXPECTED_HPP