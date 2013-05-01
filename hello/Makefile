ifneq ($(KERNELRELEASE),)

obj-m :=hello.o

else

KDIR := /home/houstar/GitRepo/driver-ok6410-3.9/linux
all:
	make -C $(KDIR) M=$(PWD) modules
install:
	cp hello.ko /srv/nfs/ok6410/lib/modules/3.8.8+
clean:
	make -C $(KDIR) M=$(PWD) clean
endif
