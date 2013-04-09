ifneq ($(KERNELRELEASE),)

obj-m :=driver_led.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
install:
	cp driver_led.ko /srv/nfs/ok6410/lib/modules/3.9.0-rc5+
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers modules*

endif
