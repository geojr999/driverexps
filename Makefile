obj-m := test.o 

KDIR  := /lib/modules/3.19.0-25-generic/build

PWD   := $(shell pwd)

default:
	    $(MAKE) -C $(KDIR) M=$(PWD) modules
