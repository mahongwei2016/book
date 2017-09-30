obj-m := globalfifo_driver.o globalfifo_device.o
KDIR := ${kernel}
all:
	echo $(shell pwd)
	make -C $(KDIR) M=$(shell pwd) modules
	cp *.ko ${rootfs}/.module
clean:
	rm -fr *.o *.ko *.order *.symvers *.mod.c .*.cmd .tmp*

