ifneq ($(KERNELRELEASE),)

obj-m :=ds18b20.o

else

KDIR := /home/houstar/GitRepo/linux
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp ds18b20.ko /srv/nfs/ok6410/lib/modules/3.8.8+/
endif
