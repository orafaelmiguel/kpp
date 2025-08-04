#ifndef KPP_NET_KERNELSOCKET_HPP
#define KPP_NET_KERNELSOCKET_HPP

#include <linux/net.h>
#include <linux/socket.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/Error.hpp>

namespace kpp
{
    class KernelSocket
    {
    private:
        struct socket* sock_;

    public:
        explicit KernelSocket(int family, int type, int protocol)
        {
            int ret = sock_create_kern(&init_net, family, type, protocol, &sock_);
            if (ret < 0)
            {
                sock_ = nullptr;
            }
        }

        ~KernelSocket()
        {
            if (sock_)
            {
                sock_release(sock_);
            }
        }

        KernelSocket(const KernelSocket&) = delete;
        KernelSocket& operator=(const KernelSocket&) = delete;

        KernelSocket(KernelSocket&& other) noexcept : sock_(other.sock_)
        {
            other.sock_ = nullptr;
        }

        KernelSocket& operator=(KernelSocket&& other) noexcept
        {
            if (this != &other)
            {
                if (sock_)
                {
                    sock_release(sock_);
                }
                sock_ = other.sock_;
                other.sock_ = nullptr;
            }
            return *this;
        }

        struct socket* native_handle() const
        {
            return sock_;
        }

        explicit operator bool() const noexcept
        {
            return sock_ != nullptr;
        }
    };

}

#endif // KPP_NET_KERNELSOCKET_HPP