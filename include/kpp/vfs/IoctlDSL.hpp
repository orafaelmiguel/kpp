#ifndef KPP_VFS_IOCTLDSL_HPP
#define KPP_VFS_IOCTLDSL_HPP

#include <linux/fs.h>
#include <kpp/core/Freestanding.hpp>
#include <kpp/vfs/UserData.hpp>

namespace kpp
{
    enum class IoctlDirection {
        NONE,
        READ,
        WRITE,
        READ_WRITE
    };

    template <unsigned int CmdNum, typename DataType, IoctlDirection Dir>
    struct IoctlCommand
    {
        static constexpr unsigned int Cmd = CmdNum;
        using Type = DataType;
        static constexpr IoctlDirection Direction = Dir;
    };

    template <typename Owner, typename... Commands>
    class IoctlHandler
    {
    public:
        long handle(Owner* owner, kpp::File file, unsigned int cmd, unsigned long arg)
        {
            bool handled = false;
            long ret = -ENOTTY;

            (void)std::initializer_list<int>{
                (process_command<Commands>(owner, cmd, arg, ret, handled), 0)...
            };

            return ret;
        }

    private:
        template <typename Command>
        void process_command(Owner* owner, unsigned int cmd, unsigned long arg, long& ret, bool& handled)
        {
            if (handled || cmd != Command::Cmd) {
                return;
            }

            using DataType = typename Command::Type;
            DataType data{};

            if constexpr (Command::Direction == IoctlDirection::WRITE || Command::Direction == IoctlDirection::READ_WRITE)
            {
                if (kpp::copy_from_user(&data, reinterpret_cast<const DataType __user*>(arg)).is_error()) {
                    ret = -EFAULT;
                    handled = true;
                    return;
                }
            }

            ret = owner->handle_ioctl(file, Command{}, data);

            if constexpr (Command::Direction == IoctlDirection::READ || Command::Direction == IoctlDirection::READ_WRITE)
            {
                if (ret == 0) {
                    if (kpp::copy_to_user(reinterpret_cast<DataType __user*>(arg), &data).is_error()) {
                        ret = -EFAULT;
                    }
                }
            }

            handled = true;
        }
    };
}

#endif // KPP_VFS_IOCTLDSL_HPP