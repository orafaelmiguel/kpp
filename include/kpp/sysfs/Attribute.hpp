#ifndef KPP_SYSFS_ATTRIBUTE_HPP
#define KPP_SYSFS_ATTRIBUTE_HPP

#include <linux/sysfs.h>
#include <kpp/core/KObject.hpp>

namespace kpp
{
    template <typename Owner>
    class Attribute
    {
    public:
        using ShowFunc = ssize_t (Owner::*)(char*);
        using StoreFunc = ssize_t (Owner::*)(const char*, size_t);

    private:
        struct kobj_attribute kobj_attr_;
        ShowFunc show_ptr_;
        StoreFunc store_ptr_;

        static ssize_t show_trampoline(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
        {
            Owner* owner = container_of(kobj, Owner, kobj.native_handle());
            Attribute* kpp_attr = container_of(attr, Attribute, kobj_attr_);

            if (kpp_attr->show_ptr_) {
                return (owner->*(kpp_attr->show_ptr_))(buf);
            }
            return -EIO;
        }

        static ssize_t store_trampoline(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
        {
            Owner* owner = container_of(kobj, Owner, kobj.native_handle());
            Attribute* kpp_attr = container_of(attr, Attribute, kobj_attr_);

            if (kpp_attr->store_ptr_) {
                return (owner->*(kpp_attr->store_ptr_))(buf, count);
            }
            return -EIO;
        }

    public:
        Attribute(const char* name, umode_t mode, ShowFunc show, StoreFunc store)
            : show_ptr_(show), store_ptr_(store)
        {
            kobj_attr_.attr.name = name;
            kobj_attr_.attr.mode = mode;
            kobj_attr_.show = show ? show_trampoline : nullptr;
            kobj_attr_.store = store ? store_trampoline : nullptr;
        }

        struct attribute* native_handle() {
            return &kobj_attr_.attr;
        }
    };

    class AttributeGroup
    {
    private:
        struct attribute_group group_;

    public:
        AttributeGroup(const char* name, struct attribute** attrs)
        {
            group_.name = name;
            group_.attrs = attrs;
        }

        struct attribute_group* native_handle()
        {
            return &group_;
        }
    };

} 

#endif // KPP_SYSFS_ATTRIBUTE_HPP