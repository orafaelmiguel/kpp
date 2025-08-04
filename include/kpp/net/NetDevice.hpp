#ifndef KPP_NET_NETDEVICE_HPP
#define KPP_NET_NETDEVICE_HPP

#include <linux/netdevice.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>

namespace kpp
{
    class NetDevice
    {
    private:
        struct net_device* dev_;

    public:
        explicit NetDevice(struct net_device* dev) : dev_(dev) {}

        ~NetDevice()
        {
            if (dev_)
            {
                dev_put(dev_);
            }
        }

        NetDevice(const NetDevice&) = delete;
        NetDevice& operator=(const NetDevice&) = delete;

        NetDevice(NetDevice&& other) noexcept : dev_(other.dev_)
        {
            other.dev_ = nullptr;
        }

        NetDevice& operator=(NetDevice&& other) noexcept
        {
            if (this != &other)
            {
                if (dev_)
                {
                    dev_put(dev_);
                }
                dev_ = other.dev_;
                other.dev_ = nullptr;
            }
            return *this;
        }

        struct net_device* native_handle() const
        {
            return dev_;
        }

        struct net_device* operator->() const
        {
            return dev_;
        }

        StringView name() const
        {
            return dev_ ? StringView(dev_->name) : StringView();
        }

        unsigned int mtu() const
        {
            return dev_ ? dev_->mtu : 0;
        }

        unsigned int flags() const
        {
            return dev_ ? dev_->flags : 0;
        }

        explicit operator bool() const noexcept
        {
            return dev_ != nullptr;
        }
    };

    namespace net
    {
        inline NetDevice find_device(StringView name)
        {
            return NetDevice(dev_get_by_name(&init_net, name.data()));
        }
    }
}

#endif // KPP_NET_NETDEVICE_HPP