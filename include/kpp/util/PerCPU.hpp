#ifndef KPP_UTIL_PERCPU_HPP
#define KPP_UTIL_PERCPU_HPP

#include <linux/percpu.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    template <typename T>
    class PerCPU
    {
    private:
        T __percpu* ptr_;

    public:
        PerCPU()
        {
            ptr_ = static_cast<T __percpu*>(alloc_percpu(sizeof(T)));
        }

        ~PerCPU()
        {
            if (ptr_)
            {
                free_percpu(ptr_);
            }
        }

        PerCPU(const PerCPU&) = delete;
        PerCPU& operator=(const PerCPU&) = delete;
        PerCPU(PerCPU&&) = delete;
        PerCPU& operator=(PerCPU&&) = delete;

        T* get()
        {
            return get_cpu_ptr(ptr_);
        }

        void put()
        {
            put_cpu_ptr(ptr_);
        }

        T* get_for_cpu(int cpu)
        {
            return per_cpu_ptr(ptr_, cpu);
        }

        explicit operator bool() const noexcept
        {
            return ptr_ != nullptr;
        }
    };
}

#endif // KPP_UTIL_PERCPU_HPP