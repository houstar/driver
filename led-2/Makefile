ifneq ($(KERNELRELEASE),)

obj-m :=led_dev.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp led_dev.ko /srv/nfs/ok6410/lib/modules/3.9.0-rc6+/
endif
