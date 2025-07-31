#ifndef KPP_CORE_FREESTANDING_HPP
#define KPP_CORE_FREESTANDING_HPP

#include <linux/stddef.h> 
#include <linux/types.h>  

namespace kpp
{
    using ::size_t;
    using ::nullptr_t;
} 

inline void* operator new(kpp::size_t size, void* p) noexcept
{
    (void)size;
    return p;
}

inline void operator delete(void* p, void* place) noexcept
{
    (void)p;
    (void)place;
}


#endif // KPP_CORE_FREESTANDING_HPP