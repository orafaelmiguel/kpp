#ifndef KPP_NET_XDP_HPP
#define KPP_NET_XDP_HPP

#include <linux/bpf.h>
#include <linux/netdevice.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/net/NetDevice.hpp>

namespace kpp
{
    class XDPAttachment
    {
    private:
        struct net_device* dev_;
        bool is_attached_ = false;

    public:
        explicit XDPAttachment(NetDevice& dev, int prog_fd, u32 flags) : dev_(dev.native_handle())
        {
            int ret = bpf_set_link_xdp_fd(dev_->ifindex, prog_fd, flags);
            if (ret == 0)
            {
                is_attached_ = true;
                dev_get(dev_);
            }
        }

        ~XDPAttachment()
        {
            if (is_attached_ && dev_)
            {
                bpf_set_link_xdp_fd(dev_->ifindex, -1, 0);
                dev_put(dev_);
            }
        }

        XDPAttachment(const XDPAttachment&) = delete;
        XDPAttachment& operator=(const XDPAttachment&) = delete;

        XDPAttachment(XDPAttachment&& other) noexcept : dev_(other.dev_), is_attached_(other.is_attached_)
        {
            other.dev_ = nullptr;
            other.is_attached_ = false;
        }

        XDPAttachment& operator=(XDPAttachment&& other) noexcept
        {
            if (this != &other)
            {
                if (is_attached_ && dev_)
                {
                    bpf_set_link_xdp_fd(dev_->ifindex, -1, 0);
                    dev_put(dev_);
                }
                dev_ = other.dev_;
                is_attached_ = other.is_attached_;
                other.dev_ = nullptr;
                other.is_attached_ = false;
            }
            return *this;
        }

        explicit operator bool() const noexcept
        {
            return is_attached_;
        }
    };
}

#endif // KPP_NET_XDP_HPP