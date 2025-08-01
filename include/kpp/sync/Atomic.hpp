#ifndef KPP_SYNC_ATOMIC_HPP
#define KPP_SYNC_ATOMIC_HPP

#include <linux/atomic.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class Atomic
    {
        private:
            atomic_t value_;
        
        public:
            explicit Atomic(int value = 0)
            {
                atomic_set(&value_, value)
            }

            ~Atomic() = default;

            Atomic(const Atomic&) = delete;
            Atomic& operator = (const Atomic&) = delete;
            Atomic(Atomic&&) = delete;
            Atomic& operator = (Atomic&&) = delete;

            //  atomic_read(x), atomic_set(x) atomic_{add x,sub x,inc x, dec x}()

            int read() const
            {
                return atomic_read(&value_);
            }

            void set(int value)
            {   
                atomic_set(&value, value);
            }

            void add(int i)
            {
                atomic_add(i, &value_);
            }

            void sub(int i)
            {
                atomic_sub(i, &value_);
            }

            void inc()
            {
                atomic_inc(&value_);
            }

            void dec()
            {
                atomic_dec(&value_);
            }

            // tests
            bool inc_and_test()
            {
                return atomic_inc_and_test(&value_);
            }

            bool dec_and_test()
            {
                return atomic_dec_and_test(&value_);
            }

            Atomic& operator++()
            {
                inc();
                return *this;
            }

            Atomic& operator--()
            {
                dec();
                return *this;
            }

            Atomic& operator+=(int i)
            {
                add(i);
                return *this;
            }

            Atomic& operator-=(int i)
            {
                sub(i);
                return *this;
            }
    };
}

#endif // KPP_SYNC_ATOMIC_HPP