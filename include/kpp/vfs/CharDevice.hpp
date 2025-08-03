#ifndef KPP_VFS_CHARDEVICE_HPP
#define KPP_VFS_CHARDEVICE_HPP

#include <linux/cdev.h>
#include <linux/fs.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class CharDevice
    {
    private:
        struct cdev cdev_;
        dev_t dev_num_;
        bool is_added_ = false;

    public:
        CharDevice(const struct file_operations* fops, dev_t dev_num) : dev_num_(dev_num)
        {
            cdev_init(&cdev_, fops);
        }

        ~CharDevice()
        {
            if (is_added_)
            {
                cdev_del(&cdev_);
            }
        }

        CharDevice(const CharDevice&) = delete;
        CharDevice& operator=(const CharDevice&) = delete;
        CharDevice(CharDevice&&) = delete;
        CharDevice& operator=(CharDevice&&) = delete;

        int add()
        {
            int ret = cdev_add(&cdev_, dev_num_, 1);
            if (ret == 0)
            {
                is_added_ = true;
            }
            return ret;
        }

        struct cdev* native_handle()
        {
            return &cdev_;
        }
    };
}

#endif // KPP_VFS_CHARDEVICE_HPP