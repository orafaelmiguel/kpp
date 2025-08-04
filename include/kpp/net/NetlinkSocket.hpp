#ifndef KPP_NET_NETLINKSOCKET_HPP
#define KPP_NET_NETLINKSOCKET_HPP

#include <linux/netlink.h>
#include <net/sock.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/net/KernelSocket.hpp>
#include <kpp/net/SKB.hpp>

namespace kpp
{
    class NetlinkSocket
    {
    private:
        struct sock* sock_;

    public:
        explicit NetlinkSocket(int protocol, struct netlink_kernel_cfg* cfg)
        {
            sock_ = netlink_kernel_create(&init_net, protocol, cfg);
        }

        ~NetlinkSocket()
        {
            if (sock_)
            {
                netlink_kernel_release(sock_);
            }
        }

        NetlinkSocket(const NetlinkSocket&) = delete;
        NetlinkSocket& operator=(const NetlinkSocket&) = delete;
        NetlinkSocket(NetlinkSocket&&) = delete;
        NetlinkSocket& operator=(NetlinkSocket&&) = delete;

        int unicast(SKB& skb, u32 portid)
        {
            return sock_ ? netlink_unicast(sock_, skb.native_handle(), portid, MSG_DONTWAIT) : -EINVAL;
        }

        int broadcast(SKB& skb, u32 portid, u32 group, gfp_t allocation)
        {
            return sock_ ? netlink_broadcast(sock_, skb.native_handle(), portid, group, allocation) : -EINVAL;
        }

        struct sock* native_handle() const
        {
            return sock_;
        }

        explicit operator bool() const noexcept
        {
            return sock_ != nullptr;
        }
    };
}

#endif // KPP_NET_NETLINKSOCKET_HPP