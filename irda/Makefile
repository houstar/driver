ifneq ($(KERNELRELEASE),)

obj-m :=irda.o

else

KDIR := /home/houstar/GitRepo/linux
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
install:
	cp irda.ko /srv/nfs/ok6410/lib/modules/3.8.8+/
endif
