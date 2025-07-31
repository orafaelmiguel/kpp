#ifndef KPP_CORE_LOG_HPP
#define KPP_CORE_LOG_HPP

#include <linux/printk.h>
#include <kpp/core/Freestanding.hpp> 

namespace kpp
{
    template <const char* LogLevel, typename... Args>
    void log(const Args&... args)
    {
        printk(LogLevel);
        ((printk("%s", " "), printk(args)), ...);
        printk("\n");
    }

} 

#endif // KPP_CORE_LOG_HPP