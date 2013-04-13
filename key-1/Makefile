ifneq ($(KERNELRELEASE),)

obj-m :=keys.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
ARCH:= arm
CROSS_COMPILE:= /usr/local/arm/4.4.1/bin/arm-linux
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp keys.ko /srv/nfs/ok6410/lib/modules/3.9.0-rc6+/
endif
