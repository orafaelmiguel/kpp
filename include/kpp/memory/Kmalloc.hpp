#ifndef KPP_MEMORY_KMALLOC_HPP
#define KPP_MEMORY_KMALLOC_HPP

#include <linux/slab.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    template <typename T>
    T* kmalloc(gfp_t flags)
    {
        return static_cast<T*>(kmalloc(sizeof(T), flags));
    }

    template <typename T>
    void kfree(T* ptr)
    {
        kfree(static_cast<const void*>(ptr));
    }

} 

#endif // KPP_MEMORY_KMALLOC_HPP