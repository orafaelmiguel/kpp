#ifndef KPP_DEBUG_DEBUGFS_HPP
#define KPP_DEBUG_DEBUGFS_HPP

#include <linux/debugfs.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>

namespace kpp
{
    namespace debugfs
    {
        class Dentry
        {
        private:
            struct dentry* dentry_;

        public:
            explicit Dentry(struct dentry* d) : dentry_(d) {}

            ~Dentry()
            {
                if (dentry_)
                {
                    debugfs_remove_recursive(dentry_);
                }
            }

            Dentry(const Dentry&) = delete;
            Dentry& operator=(const Dentry&) = delete;

            Dentry(Dentry&& other) noexcept : dentry_(other.dentry_)
            {
                other.dentry_ = nullptr;
            }

            Dentry& operator=(Dentry&& other) noexcept
            {
                if (this != &other)
                {
                    if (dentry_)
                    {
                        debugfs_remove_recursive(dentry_);
                    }
                    dentry_ = other.dentry_;
                    other.dentry_ = nullptr;
                }
                return *this;
            }

            struct dentry* native_handle() const
            {
                return dentry_;
            }

            explicit operator bool() const noexcept
            {
                return dentry_ != nullptr;
            }
        };

        inline Dentry create_dir(StringView name, Dentry& parent)
        {
            return Dentry(debugfs_create_dir(name.data(), parent.native_handle()));
        }

        inline Dentry create_file(StringView name, umode_t mode, Dentry& parent, void* data, const struct file_operations* fops)
        {
            return Dentry(debugfs_create_file(name.data(), mode, parent.native_handle(), data, fops));
        }

    } 
} 

#endif // KPP_DEBUG_DEBUGFS_HPP