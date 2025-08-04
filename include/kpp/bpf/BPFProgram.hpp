#ifndef KPP_BPF_BPFPROGRAM_HPP
#define KPP_BPF_BPFPROGRAM_HPP

#include <linux/bpf.h>
#include <linux/filter.h>
#include <kpp/core/Freestanding.hpp>

namespace kpp
{
    class BPFProgram
    {
    private:
        struct bpf_prog* prog_;

    public:
        explicit BPFProgram(struct bpf_prog* prog) : prog_(prog) {}

        ~BPFProgram()
        {
            if (prog_)
            {
                bpf_prog_put(prog_);
            }
        }

        BPFProgram(const BPFProgram&) = delete;
        BPFProgram& operator=(const BPFProgram&) = delete;

        BPFProgram(BPFProgram&& other) noexcept : prog_(other.prog_)
        {
            other.prog_ = nullptr;
        }

        BPFProgram& operator=(BPFProgram&& other) noexcept
        {
            if (this != &other)
            {
                if (prog_)
                {
                    bpf_prog_put(prog_);
                }
                prog_ = other.prog_;
                other.prog_ = nullptr;
            }
            return *this;
        }

        struct bpf_prog* native_handle() const
        {
            return prog_;
        }

        unsigned int run(const void* ctx) const
        {
            if (!prog_)
            {
                return 0;
            }
            return BPF_PROG_RUN(prog_, ctx);
        }

        explicit operator bool() const noexcept
        {
            return prog_ != nullptr;
        }
    };

    namespace bpf
    {
        inline BPFProgram program_from_fd(int fd)
        {
            struct bpf_prog* prog = bpf_prog_get(fd);
            if (IS_ERR(prog))
            {
                return BPFProgram(nullptr);
            }
            return BPFProgram(prog);
        }
    }
}

#endif // KPP_BPF_BPFPROGRAM_HPP