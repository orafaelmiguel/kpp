#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <kpp/kpp.hpp> 

class StatusModule
{
public:
    kpp::KObject kobj;
    const char* message = "K++ Milestone II is running!";
};

kpp::UniquePtr<StatusModule> g_status_module;

static ssize_t status_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    StatusModule* mod = container_of(kobj, StatusModule, kobj.native_handle());
    return sprintf(buf, "%s\n", mod->message);
}

static void status_release(struct kobject *kobj)
{
    kpp::log<KERN_INFO>("kpp_status: kobject released, freeing StatusModule.");
}

static struct kobj_attribute status_attribute = __ATTR_RO(status);

static struct attribute *attrs[] = {
    &status_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobj_type status_ktype = {
    .sysfs_ops = &sysfs_ops,
    .release = status_release,
    .default_attrs = &attr_group,
};

static struct kset *kpp_kset;

static int __init status_module_init(void)
{
    kpp::log<KERN_INFO>("kpp_status: Loading visual example module.");

    kpp_kset = kset_create_and_add("kpp_module", NULL, kernel_kobj);
    if (!kpp_kset) {
        return -ENOMEM;
    }

    g_status_module = kpp::make_unique<StatusModule>(GFP_KERNEL);
    if (!g_status_module) {
        kset_unregister(kpp_kset);
        return -ENOMEM;
    }

    int ret = g_status_module->kobj.init_and_add("status", &kpp_kset->kobj, &status_ktype);
    if (ret) {
        kset_unregister(kpp_kset);
        return ret;
    }

    kpp::log<KERN_INFO>("kpp_status: Module loaded. Check /sys/kernel/kpp_module/status");
    return 0;
}

static void __exit status_module_exit(void)
{
    g_status_module->kobj.del();
    g_status_module.release(); 

    kset_unregister(kpp_kset);

    kpp::log<KERN_INFO>("kpp_status: Module unloaded.");
}

module_init(status_module_init);
module_exit(status_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K++ Developer");
MODULE_DESCRIPTION("A K++ module with a visual sysfs result.");