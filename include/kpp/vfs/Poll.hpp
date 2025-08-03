#ifndef KPP_VFS_POLL_HPP
#define KPP_VFS_POLL_HPP

#include <linux/poll.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/vfs/VFSObjects.hpp>

namespace kpp
{
    class PollTable
    {
    private:
        struct poll_table_struct* pt_;

    public:
        explicit PollTable(struct poll_table_struct* pt) : pt_(pt) {}

        void wait(kpp::File& file, wait_queue_head_t* wq_head)
        {
            poll_wait(file.native_handle(), wq_head, pt_);
        }
    };
}

#endif // KPP_VFS_POLL_HPP