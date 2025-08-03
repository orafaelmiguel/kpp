#ifndef KPP_VFS_USERDATA_HPP
#define KPP_VFS_USERDATA_HPP

#include <linux/uaccess.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/Error.hpp>

namespace kpp
{
    template <typename T>
    [[nodiscard]] kpp::Error copy_to_user(T __user* to, const T* from)
    {
        if (copy_to_user(to, from, sizeof(T)))
        {
            return kpp::Error(-EFAULT);
        }
        return kpp::Error(0);
    }

    template <typename T>
    [[nodiscard]] kpp::Error copy_from_user(T* to, const T __user* from)
    {
        if (copy_from_user(to, from, sizeof(T)))
        {
            return kpp::Error(-EFAULT);
        }
        return kpp::Error(0);
    }

}

#endif // KPP_VFS_USERDATA_HPP