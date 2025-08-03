#include <kpp/kpp.hpp>

class EchoDriver : public kpp::FileOperations<EchoDriver>
{
private:
    static constexpr size_t BUFFER_SIZE = 256;
    char buffer_[BUFFER_SIZE] = {0};
    size_t buffer_len_ = 0;
    kpp::Mutex mutex_;

public:
    EchoDriver() : kpp::FileOperations<EchoDriver>(this) {}

    int open(kpp::Inode inode, kpp::File file)
    {
        kpp::log<KERN_INFO>("kpp_echo: Device opened.");
        return 0;
    }

    int release(kpp::Inode inode, kpp::File file)
    {
        kpp::log<KERN_INFO>("kpp_echo: Device closed.");
        return 0;
    }

    ssize_t read(kpp::File file, char* user_buf, size_t size, loff_t* offset)
    {
        kpp::ScopedLock guard(mutex_);

        if (*offset >= buffer_len_)
            return 0;

        size_t count = min(size, buffer_len_ - (size_t)*offset);
        if (kpp::copy_to_user(user_buf, buffer_ + *offset, count).is_error())
            return -EFAULT;

        *offset += count;
        return count;
    }

    ssize_t write(kpp::File file, const char* user_buf, size_t size, loff_t* offset)
    {
        kpp::ScopedLock guard(mutex_);

        size_t count = min(size, BUFFER_SIZE - 1);
        if (kpp::copy_from_user(buffer_, user_buf, count).is_error())
            return -EFAULT;

        buffer_len_ = count;
        buffer_[buffer_len_] = '\0';
        *offset = 0;

        kpp::log<KERN_INFO>("kpp_echo: Received '", kpp::StringView(buffer_, buffer_len_), "'");
        return count;
    }

protected:
    OpenFunc get_open() const override { return &EchoDriver::open; }
    ReleaseFunc get_release() const override { return &EchoDriver::release; }
    ReadFunc get_read() const override { return &EchoDriver::read; }
    WriteFunc get_write() const override { return &EchoDriver::write; }
};

static kpp::UniquePtr<EchoDriver> g_driver;
static kpp::UniquePtr<kpp::DeviceClass> g_dev_class;
static kpp::UniquePtr<kpp::CharDevice> g_char_dev;
static kpp::UniquePtr<kpp::Device> g_device;
static dev_t g_dev_num = 0;

static int __init echo_driver_init(void)
{
    kpp::log<KERN_INFO>("kpp_echo: Loading echo driver.");

    g_driver = kpp::make_unique<EchoDriver>(GFP_KERNEL);
    if (!g_driver) return -ENOMEM;

    if (alloc_chrdev_region(&g_dev_num, 0, 1, "kpp_echo") < 0) return -EFAULT;

    g_dev_class = kpp::make_unique<kpp::DeviceClass>(GFP_KERNEL, "kpp_echo_class");
    if (!g_dev_class || !*g_dev_class) return -ENOMEM;

    g_char_dev = kpp::make_unique<kpp::CharDevice>(GFP_KERNEL, g_driver->native_handle(), g_dev_num);
    if (g_char_dev->add() < 0) return -EFAULT;

    g_device = kpp::make_unique<kpp::Device>(GFP_KERNEL, *g_dev_class, g_dev_num, "kpp_echo");
    if (!g_device || !*g_device) return -ENOMEM;

    kpp::log<KERN_INFO>("kpp_echo: Driver loaded. Device created at /dev/kpp_echo");
    return 0;
}

static void __exit echo_driver_exit(void)
{
    g_device.release();
    g_char_dev.release();
    g_dev_class.release();
    unregister_chrdev_region(g_dev_num, 1);
    g_driver.release();
    kpp::log<KERN_INFO>("kpp_echo: Driver unloaded.");
}

module_init(echo_driver_init);
module_exit(echo_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K++ Developer");
MODULE_DESCRIPTION("A full character device driver example using K++.");