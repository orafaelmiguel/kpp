# KPP

KPP is a header-only, freestanding C++ library designed to bring the safety and expressiveness of modern C++ to Linux kernel module development. It provides a set of zero-cost abstractions over common kernel APIs, aiming to reduce boilerplate, improve type safety, and eliminate entire classes of common bugs like resource and memory leaks.

## Features

### Core & RAII Foundation
* **Freestanding Environment:** Provides the necessary C++ primitives (`placement new`, `nullptr_t`, `size_t`).
* **Type-Safe Utilities:** `kpp::log` for `printk`, `kpp::Error` for error codes, and `kpp::StringView` for safe string handling.
* **RAII Synchronization:** `kpp::SpinLock`, `kpp::Mutex`, `kpp::Semaphore`, `kpp::Completion`, and a generic `kpp::ScopedLock`.
* **RAII Memory Management:** `kpp::UniquePtr` and `kpp::SharedPtr` for automatic memory management.
* **Async Utilities:** RAII wrappers for `kpp::Timer` and `kpp::WorkQueue`.

### Device Model & Sysfs
* **Device Model Objects:** `kpp::Kref`, `kpp::KObject`, and `kpp::KSet` provide a robust, RAII-based foundation for creating objects recognized by the kernel.
* **Sysfs Integration:** `kpp::Attribute` and `kpp::AttributeGroup` allow C++ member functions to be used directly as `sysfs` callbacks, eliminating complex C boilerplate.
* **Debugging Utilities:** `kpp::debugfs` helpers provide a simple RAII interface for creating debug files and directories.

### Character Devices & VFS
* **File Operations Abstraction:** `kpp::FileOperations<Owner>` allows VFS callbacks (`open`, `read`, `write`, `ioctl`, `poll`, `mmap`) to be defined as C++ class member functions.
* **RAII Character Device:** A suite of classes (`kpp::CharDevice`, `kpp::DeviceClass`, `kpp::Device`) that automate the entire lifecycle of character device registration and `/dev` node creation.
* **Safe User Data Transfer:** Type-safe template wrappers `kpp::copy_to_user` and `kpp::copy_from_user` that automatically infer data size.

### Networking Stack
* **RAII Packet Management:** `kpp::SKB` provides a wrapper for `struct sk_buff` that manages its lifecycle automatically and provides safe access to network headers.
* **Netfilter Hook Management:** `kpp::NetfilterHook` ensures Netfilter hooks are always safely registered and unregistered via RAII.
* **In-Kernel Networking:** `kpp::KernelSocket` and `kpp::NetlinkSocket` provide a high-level API for performing network I/O from within the kernel.
* **Advanced Networking:** Wrappers for `net_device`, `dst_entry`, and `XDP` attachment.

## Getting Started

### Prerequisites

* A Linux environment with kernel headers installed (e.g., `linux-headers-$(uname -r)`).
* A C++ compiler (`g++`) compatible with C++17.
* `make`.

### IDE Setup (VS Code, CLion, etc.)

To get full IDE support (autocomplete, code navigation), generate a `compile_commands.json` file.

1.  Navigate to an example directory (e.g., `examples/01_hello_world`).
2.  Run the `make` command with `clang` to generate the compilation database:
    ```bash
    make -C /lib/modules/$(uname -r)/build M=$(pwd) CC=clang KCFLAGS=-Wno-error=incompatible-pointer-types-discards-qualifiers compile_commands.json
    ```
3.  Create a symbolic link to this file in the project's root directory:
    ```bash
    # From the project root
    ln -s examples/01_hello_world/compile_commands.json .
    ```
4.  Reload your IDE.

## Usage Examples

### Example 1: Basic RAII Memory Management

This module demonstrates basic logging and automatic memory management.

**`examples/01_hello_world/hello_kpp.cpp`**
```cpp
#include <kpp/kpp.hpp>

struct MyData {
    int id;
    long value;
};

kpp::UniquePtr<MyData> g_my_data;

static int __init hello_kpp_init(void)
{
    kpp::log<KERN_INFO>("K++: Hello World module loading.");
    g_my_data = kpp::make_unique<MyData>(GFP_KERNEL);
    if (!g_my_data) {
        return -ENOMEM;
    }
    g_my_data->id = 10;
    kpp::log<KERN_INFO>("K++: Data allocated with ID:", g_my_data->id);
    return 0;
}

static void __exit hello_kpp_exit(void)
{
    // g_my_data's destructor is called automatically, freeing the memory.
    kpp::log<KERN_INFO>("K++: Hello World module unloading.");
}

module_init(hello_kpp_init);
module_exit(hello_kpp_exit);
MODULE_LICENSE("GPL");
