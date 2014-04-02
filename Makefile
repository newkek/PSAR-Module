KVER=$(shell uname -r)

obj-m += ibs_measure_module.o

all:
	make -C /lib/modules/$(KVER)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVER)/build M=$(PWD) clean