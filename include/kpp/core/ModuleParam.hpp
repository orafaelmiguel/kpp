#ifndef KPP_CORE_MODULEPARAM_HPP
#define KPP_CORE_MODULEPARAM_HPP

#include <linux/moduleparam.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/core/StringView.hpp>

namespace kpp
{
    namespace detail
    {
        inline int param_set_string(const char* val, const struct kernel_param* kp)
        {
            kpp::StringView* sv = static_cast<kpp::StringView*>(kp->arg);
            *sv = kpp::StringView(val);
            return 0;
        }

        inline int param_get_string(char* buffer, const struct kernel_param* kp)
        {
            kpp::StringView* sv = static_cast<kpp::StringView*>(kp->arg);
            strncpy(buffer, sv->data(), sv->size());
            buffer[sv->size()] = '\0';
            return sv->size();
        }

        inline const struct kernel_param_ops string_view_ops = {
            .set = param_set_string,
            .get = param_get_string,
        };
    }

    class StringParam
    {
    private:
        kpp::StringView sv_;
        struct kernel_param kp_;

    public:
        StringParam(const char* name, umode_t perm) : sv_(), kp_()
        {
            kp_.name = name;
            kp_.ops = &detail::string_view_ops;
            kp_.arg = &sv_;
            kp_.perm = perm;
            module_param_cb(name, &detail::string_view_ops, &sv_, perm);
        }

        StringView value() const
        {
            return sv_;
        }
    };
}

#endif // KPP_CORE_MODULEPARAM_HPP