#ifndef KPP_NET_NETFILTERHOOK_HPP
#define KPP_NET_NETFILTERHOOK_HPP

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/net/SKB.hpp>

namespace kpp
{
    enum class NetfilterVerdict
    {
        DROP = NF_DROP,
        ACCEPT = NF_ACCEPT,
        STOLEN = NF_STOLEN,
        QUEUE = NF_QUEUE,
        REPEAT = NF_REPEAT,
    };

    template <typename Owner>
    class NetfilterHook
    {
    public:
        using HookFunc = NetfilterVerdict (Owner::*)(kpp::SKB&, const struct nf_hook_state*);

    private:
        struct nf_hook_ops ops_;
        Owner* owner_ptr_;
        HookFunc callback_ptr_;

        static unsigned int hook_trampoline(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
        {
            NetfilterHook* self = static_cast<NetfilterHook*>(priv);
            if (self && self->owner_ptr_ && self->callback_ptr_) {
                kpp::SKB kskb(skb);
                NetfilterVerdict result = (self->owner_ptr_->*(self->callback_ptr_))(kskb, state);
                return static_cast<unsigned int>(result);
            }
            return NF_ACCEPT;
        }

    public:
        NetfilterHook(Owner* owner, HookFunc callback, unsigned int hooknum, int priority)
            : owner_ptr_(owner), callback_ptr_(callback)
        {
            ops_.hook = hook_trampoline;
            ops_.priv = this;
            ops_.pf = PF_INET;
            ops_.hooknum = hooknum;
            ops_.priority = priority;
            nf_register_net_hook(&init_net, &ops_);
        }

        ~NetfilterHook()
        {
            nf_unregister_net_hook(&init_net, &ops_);
        }

        NetfilterHook(const NetfilterHook&) = delete;
        NetfilterHook& operator=(const NetfilterHook&) = delete;
        NetfilterHook(NetfilterHook&&) = delete;
        NetfilterHook& operator=(NetfilterHook&&) = delete;
    };
}

#endif // KPP_NET_NETFILTERHOOK_HPP