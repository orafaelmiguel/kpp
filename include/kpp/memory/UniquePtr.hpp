#ifndef KPP_MEMORY_UNIQUEPTR_HPP
#define KPP_MEMORY_UNIQUEPTR_HPP

#include <kpp/core/Freestanding.hpp>
#include <kpp/memory/Kmalloc.hpp>

namespace kpp
{
    template <typename T>
    struct DefaultDeleter
    {
        void operator()(T* ptr) const
        {
            kpp::kfree(ptr);
        }
    };

    template <typename T, typename Deleter = DefaultDeleter<T>>
    class UniquePtr
    {
    private:
        T* ptr_;
        Deleter deleter_;

    public:
        explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr) {}

        ~UniquePtr()
        {
            if (ptr_)
            {
                deleter_(ptr_);
            }
        }

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_)
        {
            other.ptr_ = nullptr;
        }

        UniquePtr& operator=(UniquePtr&& other) noexcept
        {
            if (this != &other)
            {
                if (ptr_)
                {
                    deleter_(ptr_);
                }
                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
            }
            return *this;
        }

        T* get() const noexcept
        {
            return ptr_;
        }

        T* release() noexcept
        {
            T* temp = ptr_;
            ptr_ = nullptr;
            return temp;
        }

        T& operator*() const noexcept
        {
            return *ptr_;
        }

        T* operator->() const noexcept
        {
            return ptr_;
        }

        explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }
    };

    template <typename T>
    UniquePtr<T> make_unique(gfp_t flags)
    {
        void* mem = kmalloc(sizeof(T), flags);
        if (!mem)
        {
            return UniquePtr<T>(nullptr);
        }
        return UniquePtr<T>(new (mem) T());
    }

}

#endif // KPP_MEMORY_UNIQUEPTR_HPP