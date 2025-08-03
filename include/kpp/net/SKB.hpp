#ifndef KPP_NET_SKB_HPP
#define KPP_NET_SKB_HPP

#include <linux/skbuff.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class SKB
    {
    private:
        struct sk_buff* skb_;

    public:
        explicit SKB(struct sk_buff* skb) : skb_(skb) {}

        ~SKB()
        {
            if (skb_)
            {
                consume_skb(skb_);
            }
        }

        SKB(const SKB&) = delete;
        SKB& operator=(const SKB&) = delete;

        SKB(SKB&& other) noexcept : skb_(other.skb_)
        {
            other.skb_ = nullptr;
        }

        SKB& operator=(SKB&& other) noexcept
        {
            if (this != &other)
            {
                if (skb_)
                {
                    consume_skb(skb_);
                }
                skb_ = other.skb_;
                other.skb_ = nullptr;
            }
            return *this;
        }

        struct sk_buff* native_handle() const
        {
            return skb_;
        }

        struct sk_buff* operator->() const
        {
            return skb_;
        }

        unsigned char* push(unsigned int len)
        {
            return skb_push(skb_, len);
        }

        unsigned char* pull(unsigned int len)
        {
            return skb_pull(skb_, len);
        }

        void trim(unsigned int len)
        {
            skb_trim(skb_, len);
        }

        void reserve(unsigned int len)
        {
            skb_reserve(skb_, len);
        }

        explicit operator bool() const noexcept
        {
            return skb_ != nullptr;
        }
    };

} 

#endif // KPP_NET_SKB_HPP