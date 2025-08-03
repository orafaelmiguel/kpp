#ifndef KPP_VFS_DEVICE_HPP
#define KPP_VFS_DEVICE_HPP

#include <linux/device.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>
#include <kpp/vfs/DeviceClass.hpp>

namespace kpp
{
    class Device
    {
    private:
        struct device* device_;
        DeviceClass& device_class_;

    public:
        Device(DeviceClass& dev_class, dev_t dev_num, StringView name) : device_class_(dev_class)
        {
            device_ = device_create(dev_class.native_handle(), NULL, dev_num, NULL, name.data());
        }

        ~Device()
        {
            if (device_)
            {
                device_destroy(device_class_.native_handle(), device_->devt);
            }
        }

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(Device&&) = delete;

        struct device* native_handle() const
        {
            return device_;
        }

        explicit operator bool() const noexcept
        {
            return device_ && !IS_ERR(device_);
        }
    };
}

#endif // KPP_VFS_DEVICE_HPP