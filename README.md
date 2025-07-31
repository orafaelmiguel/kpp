# KPP

KPP is a header-only, freestanding C++ library designed to bring the safety and expressiveness of modern C++ to Linux kernel module development. It provides a set of zero-cost abstractions over common kernel APIs, aiming to reduce boilerplate, improve type safety, and eliminate entire classes of common bugs like resource and memory leaks.

## Core 

* **Freestanding Environment:** Provides the necessary C++ primitives (`placement new`, `nullptr_t`, `size_t`) to work without the standard library.
* **Type-Safe Logging:** A convenient, variadic `kpp::log` function that wraps `printk` safely.
* **Type-Safe Error Handling:** A `kpp::Error` class that encapsulates integer error codes to prevent misuse.
* **RAII Synchronization Primitives:**
    * `kpp::SpinLock`: A wrapper for the kernel's `spinlock_t`.
    * `kpp::ScopedLock`: A generic RAII guard that ensures locks are always released.
* **RAII Memory Management:**
    * `kpp::UniquePtr`: A kernel-compatible smart pointer with exclusive ownership for memory allocated via `kmalloc`.
    * `kpp::make_unique`: A factory function for safe allocation and construction.

## Getting Started

### Prerequisites

* A Linux environment with kernel headers installed (e.g., `linux-headers-$(uname -r)`).
* A C++ compiler (`g++`) compatible with C++17.
* `make`.

### Build System

K++ integrates with the standard kernel build system. To build a module that uses the library, you will need a `Makefile` in your module's directory. See the example below for a template.

### IDE Setup (VS Code, CLion, etc.)

To get full IDE support (autocomplete, code navigation), you need to generate a `compile_commands.json` file. This allows language servers like `clangd` to understand the project structure.

1.  Navigate to an example directory (e.g., `examples/01_hello_world`).
2.  Run the `make` command with `clang` to generate the compilation database:
    ```bash
    make -C /lib/modules/$(uname -r)/build M=$(pwd) CC=clang KCFLAGS=-Wno-error=incompatible-pointer-types-discards-qualifiers compile_commands.json
    ```
3.  Create a symbolic link to this file in the project's root directory so your IDE can find it:
    ```bash
    # From the project root
    ln -s examples/01_hello_world/compile_commands.json .
    ```
4.  Reload your IDE. You should now have full language support.

## Usage Example

Here is a simple kernel module that demonstrates logging and memory management with K++.

**`examples/01_hello_world/hello_kpp.cpp`**
```cpp
#include <kpp/kpp.hpp> // Main convenience header for K++

// A simple structure for our example
struct MyData {
    int id;
    long value;
};

// Global pointer to our allocated data, managed by UniquePtr
kpp::UniquePtr<MyData> g_my_data;

static int __init hello_kpp_init(void)
{
    kpp::log<KERN_INFO>("K++ module loading.");

    // Allocate and construct MyData using the kpp::make_unique helper.
    // Memory will be automatically freed when the module unloads.
    g_my_data = kpp::make_unique<MyData>(GFP_KERNEL);

    if (!g_my_data) {
        kpp::log<KERN_ERR>("Failed to allocate memory.");
        return -ENOMEM;
    }

    // Initialize the data using smart pointer syntax
    g_my_data->id = 10;
    g_my_data->value = 12345L;

    kpp::log<KERN_INFO>("MyData allocated and initialized. ID:", g_my_data->id);

    return 0;
}

static void __exit hello_kpp_exit(void)
{
    // The g_my_data UniquePtr goes out of scope when the module unloads.
    // Its destructor is called, which automatically calls kfree on the pointer.
    // No manual cleanup is needed.
    kpp::log<KERN_INFO>("K++ module unloading. Memory will be freed automatically.");
}

module_init(hello_kpp_init);
module_exit(hello_kpp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K++ Developer");
MODULE_DESCRIPTION("A simple K++ example module.");
