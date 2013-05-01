ifneq ($(KERNELRELEASE),)

obj-m :=leds.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp leds.ko /srv/nfs/ok6410/lib/modules/3.8.8+/
endif
