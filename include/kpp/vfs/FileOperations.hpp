#ifndef KPP_VFS_FILEOPERATIONS_HPP
#define KPP_VFS_FILEOPERATIONS_HPP

#include <linux/fs.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>
#include <kpp/vfs/Poll.hpp>
#include <kpp/vfs/VFSObjects.hpp>

namespace kpp
{
    template <typename Owner>
    class FileOperations
    {
    public:
        using OpenFunc = int (Owner::*)(Inode, File);
        using ReleaseFunc = int (Owner::*)(Inode, File);
        using ReadFunc = ssize_t (Owner::*)(File, char*, size_t, loff_t*);
        using WriteFunc = ssize_t (Owner::*)(File, const char*, size_t, loff_t*);
        using IoctlFunc = long (Owner::*)(File, unsigned int, unsigned long);
        using PollFunc = __poll_t (Owner::*)(File, PollTable);

    private:
        struct file_operations fops_;
        Owner* owner_ptr_;

        static int open_trampoline(struct inode *i, struct file *f)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
            if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_open();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(Inode(i), File(f));
                }
            }
            return 0;
        }

        static int release_trampoline(struct inode *i, struct file *f)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
             if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_release();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(Inode(i), File(f));
                }
            }
            return 0;
        }

        static ssize_t read_trampoline(struct file *f, char __user *buf, size_t size, loff_t *off)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
             if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_read();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(File(f), buf, size, off);
                }
            }
            return -EIO;
        }

        static ssize_t write_trampoline(struct file *f, const char __user *buf, size_t size, loff_t *off)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
             if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_write();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(File(f), buf, size, off);
                }
            }
            return -EIO;
        }

        static long ioctl_trampoline(struct file *f, unsigned int cmd, unsigned long arg)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
            if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_ioctl();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(File(f), cmd, arg);
                }
            }
            return -ENOTTY;
        }

        static __poll_t poll_trampoline(struct file *f, struct poll_table_struct *pt)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
            if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_poll();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(File(f), PollTable(pt));
                }
            }
            return DEFAULT_POLLMASK;
        }

    protected:
        virtual OpenFunc get_open() const { return nullptr; }
        virtual ReleaseFunc get_release() const { return nullptr; }
        virtual ReadFunc get_read() const { return nullptr; }
        virtual WriteFunc get_write() const { return nullptr; }
        virtual IoctlFunc get_ioctl() const { return nullptr; }
        virtual PollFunc get_poll() const { return nullptr; }

    public:
        FileOperations(Owner* owner) : owner_ptr_(owner)
        {
            memset(&fops_, 0, sizeof(fops_));
            fops_.owner = THIS_MODULE;
            fops_.open = get_open() ? open_trampoline : nullptr;
            fops_.release = get_release() ? release_trampoline : nullptr;
            fops_.read = get_read() ? read_trampoline : nullptr;
            fops_.write = get_write() ? write_trampoline : nullptr;
            fops_.unlocked_ioctl = get_ioctl() ? ioctl_trampoline : nullptr;
            fops_.poll = get_poll() ? poll_trampoline : nullptr;
        }

        const struct file_operations* native_handle() const
        {
            return &fops_;
        }
    };
}

#endif // KPP_VFS_FILEOPERATIONS_HPP