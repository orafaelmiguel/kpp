obj-m := echo_driver.o
echo_driver-objs := echo_driver.o

EXTRA_CXXFLAGS := -fno-exceptions -fno-rtti -std=c++17
ccflags-y += $(EXTRA_CXXFLAGS)
ccflags-y += -I$(src)/../../include

KDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: all clean

all:
	$(MAKE) -C "$(KDIR)" M="$(PWD)" CC=g++ modules

clean:
	$(MAKE) -C "$(KDIR)" M="$(PWD)" clean