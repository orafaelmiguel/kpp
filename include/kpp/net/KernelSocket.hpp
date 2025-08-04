#ifndef KPP_NET_KERNELSOCKET_HPP
#define KPP_NET_KERNELSOCKET_HPP

#include <linux/net.h>
#include <linux/socket.h>
#include <linux/in.h>
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

        explicit KernelSocket(struct socket* existing_sock = nullptr) : sock_(existing_sock) {}

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

        int connect(struct sockaddr* addr, int addr_len, int flags)
        {
            return sock_ ? sock_->ops->connect(sock_, addr, addr_len, flags) : -EINVAL;
        }

        int bind(struct sockaddr* addr, int addr_len)
        {
            return sock_ ? sock_->ops->bind(sock_, addr, addr_len) : -EINVAL;
        }

        int listen(int backlog)
        {
            return sock_ ? sock_->ops->listen(sock_, backlog) : -EINVAL;
        }

        KernelSocket accept()
        {
            struct socket* new_sock = nullptr;
            int ret = sock_ ? sock_->ops->accept(sock_, new_sock, 0, false) : -EINVAL;
            if (ret < 0)
            {
                return KernelSocket(nullptr);
            }
            return KernelSocket(new_sock);
        }

        ssize_t send(const void* buffer, size_t length, int flags)
        {
            if (!sock_) return -EINVAL;

            struct kvec vec = { .iov_base = const_cast<void*>(buffer), .iov_len = length };
            struct msghdr msg = { .msg_flags = flags };
            return kernel_sendmsg(sock_, &msg, &vec, 1, length);
        }

        ssize_t receive(void* buffer, size_t length, int flags)
        {
            if (!sock_) return -EINVAL;

            struct kvec vec = { .iov_base = buffer, .iov_len = length };
            struct msghdr msg = { .msg_flags = flags };
            return kernel_recvmsg(sock_, &msg, &vec, 1, length, flags);
        }

        void shutdown()
        {
            if (sock_)
            {
                kernel_sock_shutdown(sock_, SHUT_RDWR);
            }
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