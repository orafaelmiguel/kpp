#ifndef KPP_CORE_STRINGVIEW_HPP
#define KPP_CORE_STRINGVIEW_HPP

#include <linux/string.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class StringView
    {
    private:
        const char* data_;
        size_t size_;

    public:
        constexpr StringView() : data_(nullptr), size_(0) {}
        constexpr StringView(const char* s, size_t count) : data_(s), size_(count) {}
        constexpr StringView(const char* s) : data_(s), size_(s ? strlen(s) : 0) {}

        constexpr const char* data() const noexcept { return data_; }
        constexpr size_t size() const noexcept { return size_; }
        constexpr bool empty() const noexcept { return size_ == 0; }

        constexpr const char& operator[](size_t pos) const { return data_[pos]; }
    };

    inline bool operator==(StringView lhs, StringView rhs) noexcept
    {
        return (lhs.size() == rhs.size()) && (strncmp(lhs.data(), rhs.data(), lhs.size()) == 0);
    }

    inline bool operator!=(StringView lhs, StringView rhs) noexcept
    {
        return !(lhs == rhs);
    }

} 

#endif // KPP_CORE_STRINGVIEW_HPP