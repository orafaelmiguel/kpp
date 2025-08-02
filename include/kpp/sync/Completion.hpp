#ifndef KPP_SYNC_COMPLETION_HPP
#define KPP_SYNC_COMPLETION_HPP

#include <linux/completion.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Completion
    {
    private:
        struct completion completion_;

    public:
        Completion()
        {
            init_completion(&completion_);
        }

        ~Completion() = default;

        Completion(const Completion&) = delete;
        Completion& operator=(const Completion&) = delete;
        Completion(Completion&&) = delete;
        Completion& operator=(Completion&&) = delete;

        void wait()
        {
            wait_for_completion(&completion_);
        }

        void complete()
        {
            complete(&completion_);
        }

        void complete_all()
        {
            complete_all(&completion_);
        }
    };

} 

#endif // KPP_SYNC_COMPLETION_HPP