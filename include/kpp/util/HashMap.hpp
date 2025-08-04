#ifndef KPP_UTIL_HASHMAP_HPP
#define KPP_UTIL_HASHMAP_HPP

#include <linux/string.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/util/List.hpp>
#include <kpp/memory/UniquePtr.hpp>

namespace kpp
{
    template <typename Key, typename Value, typename Hasher>
    class HashMap
    {
    private:
        struct Node
        {
            struct list_head list_node;
            Key key;
            Value value;
        };

        kpp::UniquePtr<kpp::List[]> buckets_;
        size_t bucket_count_;
        Hasher hasher_;

    public:
        explicit HashMap(size_t bucket_count) : bucket_count_(bucket_count)
        {
            buckets_ = kpp::make_unique<kpp::List[]>(GFP_KERNEL, bucket_count);
        }

        kpp::Error insert(const Key& key, const Value& value)
        {
            if (!buckets_) return kpp::Error(-ENOMEM);

            size_t hash = hasher_(key) % bucket_count_;
            kpp::List& bucket = buckets_[hash];

            auto node_ptr = kpp::make_unique<Node>(GFP_KERNEL);
            if (!node_ptr) return kpp::Error(-ENOMEM);

            node_ptr->key = key;
            node_ptr->value = value;

            bucket.add_tail(&node_ptr.release()->list_node);
            return kpp::Error(0);
        }

        Value* find(const Key& key)
        {
            if (!buckets_) return nullptr;

            size_t hash = hasher_(key) % bucket_count_;
            kpp::List& bucket = buckets_[hash];

            for (auto& node : bucket)
            {
                Node* current = container_of(&node, Node, list_node);
                if (current->key == key)
                {
                    return &current->value;
                }
            }

            return nullptr;
        }
    };
}

#endif // KPP_UTIL_HASHMAP_HPP