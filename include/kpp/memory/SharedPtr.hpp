#ifndef KPP_MEMORY_SHAREDPTR_HPP
#define KPP_MEMORY_SHAREDPTR_HPP

#include <linux/slab.h>
#include <linux/atomic.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/memory/Kmalloc.hpp>

namespace kpp
{
    namespace detail
    {
        struct ControlBlock
        {
            atomic_t shared_count;
        };

        template <typename T>
        struct ControlBlockImpl : public ControlBlock
        {
            T object;
        };
    }

    template <typename T>
    class SharedPtr
    {
    private:
        T* ptr_;
        detail::ControlBlock* cb_;

        void release()
        {
            if (cb_ && atomic_dec_and_test(&cb_->shared_count))
            {
                ptr_->~T();
                kfree(cb_);
            }
        }

    public:
        SharedPtr() : ptr_(nullptr), cb_(nullptr) {}

        explicit SharedPtr(T* ptr, detail::ControlBlock* cb) : ptr_(ptr), cb_(cb) {}

        ~SharedPtr()
        {
            release();
        }

        SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
        {
            if (cb_)
            {
                atomic_inc(&cb_->shared_count);
            }
        }

        SharedPtr& operator=(const SharedPtr& other) noexcept
        {
            if (this != &other)
            {
                release();
                ptr_ = other.ptr_;
                cb_ = other.cb_;
                if (cb_)
                {
                    atomic_inc(&cb_->shared_count);
                }
            }
            return *this;
        }

        SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_), cb_(other.cb_)
        {
            other.ptr_ = nullptr;
            other.cb_ = nullptr;
        }

        SharedPtr& operator=(SharedPtr&& other) noexcept
        {
            if (this != &other)
            {
                release();
                ptr_ = other.ptr_;
                cb_ = other.cb_;
                other.ptr_ = nullptr;
                other.cb_ = nullptr;
            }
            return *this;
        }

        T* get() const noexcept
        {
            return ptr_;
        }

        long use_count() const noexcept
        {
            return cb_ ? atomic_read(&cb_->shared_count) : 0;
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

    template <typename T, typename... Args>
    SharedPtr<T> make_shared(gfp_t flags, Args&&... args)
    {
        using ControlBlockType = detail::ControlBlockImpl<T>;
        void* mem = kmalloc(sizeof(ControlBlockType), flags);
        if (!mem)
        {
            return SharedPtr<T>();
        }

        ControlBlockType* cb_impl = static_cast<ControlBlockType*>(mem);
        atomic_set(&cb_impl->shared_count, 1);

        T* object_ptr = new (&cb_impl->object) T(static_cast<Args&&>(args)...);

        return SharedPtr<T>(object_ptr, cb_impl);
    }

} 

#endif // KPP_MEMORY_SHAREDPTR_HPP