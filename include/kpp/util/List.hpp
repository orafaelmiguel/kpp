#ifndef KPP_UTIL_LIST_HPP
#define KPP_UTIL_LIST_HPP

#include <linux/list.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class List
    {
    public:
        class Iterator
        {
        private:
            struct list_head* node_;
        public:
            explicit Iterator(struct list_head* node) : node_(node) {}

            struct list_head* operator*() const { return node_; }
            Iterator& operator++() { node_ = node_->next; return *this; }
            bool operator!=(const Iterator& other) const { return node_ != other.node_; }
        };

    private:
        struct list_head head_;

    public:
        List()
        {
            INIT_LIST_HEAD(&head_);
        }

        ~List() = default;

        List(const List&) = delete;
        List& operator=(const List&) = delete;
        List(List&&) = delete;
        List& operator=(List&&) = delete;

        [[nodiscard]] bool empty() const
        {
            return list_empty(&head_);
        }

        void add(struct list_head* node)
        {
            list_add(node, &head_);
        }

        void add_tail(struct list_head* node)
        {
            list_add_tail(node, &head_);
        }

        static void del(struct list_head* node)
        {
            list_del(node);
        }

        [[nodiscard]] Iterator begin() const
        {
            return Iterator(head_.next);
        }

        [[nodiscard]] Iterator end() const
        {
            return Iterator(const_cast<struct list_head*>(&head_));
        }
    };

} 

#endif // KPP_UTIL_LIST_HPP