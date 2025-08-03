#ifndef KPP_VFS_FILEOPERATIONS_HPP
#define KPP_VFS_FILEOPERATIONS_HPP

#include <linux/fs.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>

namespace kpp
{
    template <typename Owner>
    class FileOperations
    {
    public:
        using OpenFunc = int (Owner::*)(struct inode*, struct file*);
        using ReleaseFunc = int (Owner::*)(struct inode*, struct file*);
        using ReadFunc = ssize_t (Owner::*)(struct file*, char*, size_t, loff_t*);
        using WriteFunc = ssize_t (Owner::*)(struct file*, const char*, size_t, loff_t*);

    private:
        struct file_operations fops_;
        Owner* owner_ptr_;

        static int open_trampoline(struct inode *i, struct file *f)
        {
            auto* kpp_fops = static_cast<const FileOperations*>(f->f_op);
            if (kpp_fops && kpp_fops->owner_ptr_) {
                auto func = kpp_fops->get_open();
                if (func) {
                    return (kpp_fops->owner_ptr_->*func)(i, f);
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
                    return (kpp_fops->owner_ptr_->*func)(i, f);
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
                    return (kpp_fops->owner_ptr_->*func)(f, buf, size, off);
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
                    return (kpp_fops->owner_ptr_->*func)(f, buf, size, off);
                }
            }
            return -EIO;
        }

    protected:
        virtual OpenFunc get_open() const { return nullptr; }
        virtual ReleaseFunc get_release() const { return nullptr; }
        virtual ReadFunc get_read() const { return nullptr; }
        virtual WriteFunc get_write() const { return nullptr; }

    public:
        FileOperations(Owner* owner) : owner_ptr_(owner)
        {
            memset(&fops_, 0, sizeof(fops_));
            fops_.owner = THIS_MODULE;
            fops_.open = get_open() ? open_trampoline : nullptr;
            fops_.release = get_release() ? release_trampoline : nullptr;
            fops_.read = get_read() ? read_trampoline : nullptr;
            fops_.write = get_write() ? write_trampoline : nullptr;
        }

        const struct file_operations* native_handle() const
        {
            return &fops_;
        }
    };
}

#endif // KPP_VFS_FILEOPERATIONS_HPP