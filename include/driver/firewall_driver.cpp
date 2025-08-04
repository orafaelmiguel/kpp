#include <kpp/kpp.hpp>

class Firewall
{
private:
    kpp::UniquePtr<kpp::NetfilterHook<Firewall>> hook_;
    unsigned short port_to_block_ = 80; // p80 blocked

public:
    Firewall()
    {
        kpp::log<KERN_INFO>("kpp_firewall: Initializing firewall to block TCP port ", port_to_block_);
        hook_ = kpp::make_unique<kpp::NetfilterHook<Firewall>>(GFP_KERNEL,
            this,
            &Firewall::hook_callback,
            NF_INET_PRE_ROUTING,
            NF_IP_PRI_FIRST
        );
    }

    kpp::NetfilterVerdict hook_callback(kpp::SKB& kskb, const struct nf_hook_state* state)
    {
        auto iph = kskb.ip_header();
        if (!iph || iph->protocol != IPPROTO_TCP) {
            return kpp::NetfilterVerdict::ACCEPT;
        }

        auto tcph = kskb.tcp_header();
        if (!tcph) {
            return kpp::NetfilterVerdict::ACCEPT;
        }

        if (ntohs(tcph->dest) == port_to_block_) {
            kpp::log<KERN_INFO>("kpp_firewall: Dropping TCP packet for port ", port_to_block_);
            return kpp::NetfilterVerdict::DROP;
        }

        return kpp::NetfilterVerdict::ACCEPT;
    }
};

static kpp::UniquePtr<Firewall> g_firewall;

static int __init firewall_driver_init(void)
{
    g_firewall = kpp::make_unique<Firewall>(GFP_KERNEL);
    if (!g_firewall) {
        return -ENOMEM;
    }
    kpp::log<KERN_INFO>("kpp_firewall: Netfilter hook registered.");
    return 0;
}

static void __exit firewall_driver_exit(void)
{
    g_firewall.release(); 
                
    kpp::log<KERN_INFO>("kpp_firewall: Netfilter hook unregistered.");
}

module_init(firewall_driver_init);
module_exit(firewall_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K++ Developer");
MODULE_DESCRIPTION("A simple Netfilter firewall example using K++.");