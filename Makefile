KVER = $(shell uname -r)
PWD  = $(shell pwd)
KDIR = /lib/modules/$(KVER)/build

obj-m := ibs_measure_module.o
ibs_measure_module-y := src/ibs_measure_module.o src/ibs_samples_management.o src/order.o


ccflags-y := -I$(src)/include

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
