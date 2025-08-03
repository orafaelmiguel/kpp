#ifndef KPP_VFS_DEVICECLASS_HPP
#define KPP_VFS_DEVICECLASS_HPP

#include <linux/device.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>

namespace kpp
{
    class DeviceClass
    {
    private:
        struct class* class_;

    public:
        explicit DeviceClass(StringView name)
        {
            class_ = class_create(name.data());
        }

        ~DeviceClass()
        {
            if (class_)
            {
                class_destroy(class_);
            }
        }

        DeviceClass(const DeviceClass&) = delete;
        DeviceClass& operator=(const DeviceClass&) = delete;

        DeviceClass(DeviceClass&& other) noexcept : class_(other.class_)
        {
            other.class_ = nullptr;
        }

        DeviceClass& operator=(DeviceClass&& other) noexcept
        {
            if (this != &other)
            {
                if (class_)
                {
                    class_destroy(class_);
                }
                class_ = other.class_;
                other.class_ = nullptr;
            }
            return *this;
        }

        struct class* native_handle() const
        {
            return class_;
        }

        explicit operator bool() const noexcept
        {
            return class_ && !IS_ERR(class_);
        }
    };
}

#endif // KPP_VFS_DEVICECLASS_HPP