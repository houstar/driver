#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-bank-m.h>

#define DEVICE_NAME "leds"
static int major;
static struct cdev *led_dev;
static struct class *led_class;
module_param(major,int,0);

ssize_t open(struct inode *inode,struct file *filep)
{

	return 0;
}

ssize_t read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

ssize_t write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

static long unlocked_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case 0:
		case 1:
			if(arg >3 || arg < 0) return -EINVAL;
			if(cmd == 0)//close light
				__raw_writel(__raw_readl(S3C64XX_GPMDAT)|(1<<arg),S3C64XX_GPMDAT);
			else //open light
				__raw_writel(__raw_readl(S3C64XX_GPMDAT)&(~(1<<arg)),S3C64XX_GPMDAT);
			return 0;
		default:
			return -EINVAL;
	}
}
/* Now we'll close all light */
static int release(struct inode *inode,struct file *filp)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open,
	.read = read,
	.write = write,
	.unlocked_ioctl = unlocked_ioctl,
	.release = release,
};


static void led_setup_cdev(struct cdev *dev,int minor,const struct file_operations *fops)
{
	int err,devno = MKDEV(major,minor);
	dev = cdev_alloc();
	cdev_init(dev,fops);
	dev->owner = THIS_MODULE;
	dev->ops = fops;
	err = cdev_add(dev,devno,1);
	if(err)
	{
		printk(KERN_NOTICE"Error %d adding led %d",err,minor);
	}
}
static int __init led_init(void)
{
	int result;
	dev_t dev = MKDEV(major,0);
	if(major)
	{
		result = register_chrdev_region(dev,1,DEVICE_NAME);
	}
	else
	{
		result = alloc_chrdev_region(&dev,0,1,DEVICE_NAME);
		major=MAJOR(dev);
	}
	if(result < 0)
	{
		printk(KERN_WARNING"LED:Unable to register major %d\n",major);
		return result;
	}

	led_setup_cdev(led_dev,0,&fops);
	led_class = class_create(THIS_MODULE,"led_class1");
    if(IS_ERR(led_class))
	{
		printk(KERN_INFO"Create class error\n");
		return -1;
	}
	device_create(led_class,NULL,dev,NULL,"leds","%d",MINOR(dev));

	printk("the major of the led device is %d\n",major);

	return 0;

}

static void __exit led_exit(void)
{
	cdev_del(led_dev);
	unregister_chrdev_region(MKDEV(major,0),1);
	
	device_destroy(led_class,MKDEV(major,0));
	class_destroy(led_class);
	printk("led driver uninstalled!\n");
}

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

module_init(led_init);
module_exit(led_exit);
