ifneq ($(KERNELRELEASE),)

obj-m :=leds.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
ARCH:= arm
CROSS_COMPILE:= /usr/local/arm/4.4.1/bin/arm-linux
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order
install:
	cp leds.ko /srv/nfs/ok6410/lib/modules/3.9.0-rc5+/
endif
