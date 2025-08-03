#ifndef KPP_VFS_VFSOBJECTS_HPP
#define KPP_VFS_VFSOBJECTS_HPP

#include <linux/fs.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Inode
    {
    private:
        struct inode* inode_;

    public:
        explicit Inode(struct inode* i) : inode_(i) {}

        struct inode* native_handle() const
        {
            return inode_;
        }

        struct inode* operator->() const
        {
            return inode_;
        }

        explicit operator bool() const noexcept
        {
            return inode_ != nullptr;
        }
    };

    class File
    {
    private:
        struct file* file_;

    public:
        explicit File(struct file* f) : file_(f) {}

        struct file* native_handle() const
        {
            return file_;
        }

        struct file* operator->() const
        {
            return file_;
        }

        explicit operator bool() const noexcept
        {
            return file_ != nullptr;
        }
    };

}

#endif // KPP_VFS_VFSOBJECTS_HPP