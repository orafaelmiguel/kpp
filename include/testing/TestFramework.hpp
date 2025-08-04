#ifndef KPP_TESTING_TESTFRAMEWORK_HPP
#define KPP_TESTING_TESTFRAMEWORK_HPP

#include <kpp/core/Freestanding.hpp>
#include <kpp/core/Log.hpp>

namespace kpp::testing
{
    inline bool run_test(void (*test_func)(), const char* test_name)
    {
        kpp::log<KERN_INFO>("[ RUN      ] ", test_name);
        test_func();
        kpp::log<KERN_INFO>("[       OK ] ", test_name);
        return true;
    }
}

#define KPP_ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            kpp::log<KERN_ERR>("[  FAILED  ] %s:%d: Assertion failed: " #condition " is not true", __FILE__, __LINE__); \
            panic("K++ Test assertion failed"); \
        } \
    } while (0)

#define KPP_ASSERT_EQ(val1, val2) \
    do { \
        if (!((val1) == (val2))) { \
            kpp::log<KERN_ERR>("[  FAILED  ] %s:%d: Assertion failed: " #val1 " (actual) != " #val2 " (expected)", __FILE__, __LINE__); \
            panic("K++ Test assertion failed"); \
        } \
    } while (0)

#define TEST(test_suite, test_name) \
    void test_suite##_##test_name##_test(); \
    namespace { bool registered_##test_suite##_##test_name = (kpp::testing::run_test(test_suite##_##test_name##_test, #test_suite "." #test_name), true); } \
    void test_suite##_##test_name##_test()

#endif // KPP_TESTING_TESTFRAMEWORK_HPP