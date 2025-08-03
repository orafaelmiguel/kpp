#ifndef KPP_VFS_PAGE_HPP
#define KPP_VFS_PAGE_HPP

#include <linux/mm.h>
#include <linux/highmem.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Page
    {
    private:
        struct page* page_;

    public:
        class MappedPtr
        {
        private:
            void* addr_;
        public:
            explicit MappedPtr(const Page& p) { addr_ = kmap(p.native_handle()); }
            ~MappedPtr() { if (addr_) kunmap(page_from_virt(addr_)); }

            MappedPtr(const MappedPtr&) = delete;
            MappedPtr& operator=(const MappedPtr&) = delete;
            MappedPtr(MappedPtr&&) = delete;
            MappedPtr& operator=(MappedPtr&&) = delete;

            template<typename T = void>
            T* get() const { return static_cast<T*>(addr_); }
        };

        explicit Page(struct page* p) : page_(p) {}

        struct page* native_handle() const
        {
            return page_;
        }

        struct page* operator->() const
        {
            return page_;
        }

        MappedPtr map() const
        {
            return MappedPtr(*this);
        }

        explicit operator bool() const noexcept
        {
            return page_ != nullptr;
        }
    };

} 

#endif // KPP_VFS_PAGE_HPP