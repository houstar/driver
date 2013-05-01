/*
 * irda.c
 * Author:houstar
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/gpio.h>

#include <asm/irq.h>
#include <plat/gpio-cfg.h>
#include <mach/hardware.h>
#include <mach/regs-gpio.h>
	
#define DEVICE_NAME "irda"

static int major;
module_param(major,int,0);

ssize_t dev_open(struct inode *inode,struct file *filep)
{
	/* Configure GPE(1) ad input mode */
	s3c_gpio_cfgpin(S3C64XX_GPE(1),0x0);
	return 0;
}

ssize_t dev_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	*buf = gpio_get_value(S3C64XX_GPE(1));
	return 1;
}

ssize_t dev_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

static long unlocked_dev_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	return 0;
}
/* Now we'll close all light */
static int dev_release(struct inode *inode,struct file *filp)
{
	return 0;
}

static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = unlocked_dev_ioctl,
	.release = dev_release,
};

static struct cdev dev_cdev;

static int __init dev_init(void)
{
	int result;
	dev_t devno = MKDEV(major,0);
	struct class *irda_class;

	if(major)
	{
		result = register_chrdev_region(devno,1,DEVICE_NAME);
	}
	else
	{
		result =alloc_chrdev_region(&devno,0,1,DEVICE_NAME);
	}
	if(result < 0)
	{
		printk(KERN_WARNING"%s:Uable to register major %d\n",DEVICE_NAME,major);
		return result;
	}

	cdev_init(&dev_cdev,&dev_fops);
	result = cdev_add(&dev_cdev,devno,1);
	if(result)
	{
		printk(KERN_NOTICE"Error %d adding irda",result);
		return result;
	}
	irda_class = class_create(THIS_MODULE,"irda_class");
	device_create(irda_class,NULL,devno,"irda","irda%d",0);

	printk(DEVICE_NAME "  initialized\n");

	return 0;
}

static void __exit dev_exit(void)
{
	cdev_del(&dev_cdev);
	unregister_chrdev_region(MKDEV(major,0),1);
}

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);
