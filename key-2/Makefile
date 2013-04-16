ifneq ($(KERNELRELEASE),)

obj-m :=misc_key.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp misc_key.ko /srv/nfs/ok6410/lib/modules/3.9.0-rc6+/
endif
