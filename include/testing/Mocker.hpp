#ifndef KPP_TESTING_MOCKER_HPP
#define KPP_TESTING_MOCKER_HPP

#include <kpp/core/Freestanding.hpp>
#include <kpp/core/Log.hpp>

namespace kpp::testing
{
    template <typename ReturnType>
    struct MockState
    {
        bool is_active = false;
        ReturnType forced_return_value;
        int call_count = 0;

        void set_return(ReturnType value)
        {
            is_active = true;
            forced_return_value = value;
        }

        void reset()
        {
            is_active = false;
            call_count = 0;
        }
    };
}

#endif // KPP_TESTING_MOCKER_HPP