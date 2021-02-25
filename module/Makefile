MODULE_NAME := com_kmodule
obj-m := $(MODULE_NAME).o
KERNEL_VER := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(KERNEL_VER)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
ins:
	sudo insmod $(MODULE_NAME).ko
rm:
	sudo rmmod $(MODULE_NAME)
ir:
	sudo rmmod $(MODULE_NAME)
	sudo insmod $(MODULE_NAME).ko
