#ifndef KPP_CORE_KOBJECT_HPP
#define KPP_CORE_KOBJECT_HPP

#include <linux/kobject.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class KObject
    {
    private:
        struct kobject kobj_;

    public:
        KObject() = default;
        ~KObject() = default;

        KObject(const KObject&) = delete;
        KObject& operator=(const KObject&) = delete;
        KObject(KObject&&) = delete;
        KObject& operator=(KObject&&) = delete;

        int init_and_add(const char* name, struct kobject* parent, struct kobj_type* type)
        {
            kobject_init(&kobj_, type);
            return kobject_add(&kobj_, parent, "%s", name);
        }

        void del()
        {
            kobject_del(&kobj_);
        }

        void get()
        {
            kobject_get(&kobj_);
        }

        void put()
        {
            kobject_put(&kobj_);
        }

        struct kobject* native_handle()
        {
            return &kobj_;
        }
    };

} 

#endif // KPP_CORE_KOBJECT_HPP