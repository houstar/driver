#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <plat/gpio-cfg.h>

#define DEVICE_NAME "leds"
#define LED_ON 1
#define LED_OFF 0
static int major;
static struct cdev *led_dev;
static struct class *led_class;
module_param(major,int,0);

ssize_t led_open(struct inode *inode,struct file *filep)
{
	unsigned tmp;
	for(tmp=0;tmp<4;tmp++){
		s3c_gpio_cfgpin(S3C64XX_GPM(tmp),S3C_GPIO_SFN(1));
		gpio_set_value(S3C64XX_GPM(tmp),1);
		s3c_gpio_setpull(S3C64XX_GPM(tmp),S3C_GPIO_SFN(2));
	}
	return 0;
}

ssize_t led_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}
/*
 * buf[0] which led
 *  0 -GPM0
 *  1 -GPM1
 *  2 -GPM2
 *  3 -GPM3
 *  buf[1] -0:off 1:on
 */
ssize_t led_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	char which;
	char off;
	char ker_buf[2];
	copy_from_user(ker_buf,buf,2);
	which = ker_buf[0];
	off = ker_buf[1];

	if(which <0 || which > 3)
	{
		return -EINVAL;
	}
	else
	{
		if(off)
			gpio_set_value(S3C64XX_GPM(which),1);
		else
			gpio_set_value(S3C64XX_GPM(which),0);
	}
	return 2;
}

static long unlocked_led_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	if(arg >3 || arg < 0) return -EINVAL;
	switch(cmd)
	{
		case LED_OFF:
			gpio_set_value(S3C64XX_GPM(arg),1);
			return 0;
		case LED_ON:
			gpio_set_value(S3C64XX_GPM(arg),0);
			return 0;
		default:
			return -EINVAL;
	}
}
/* Now we'll close all light */
static int led_release(struct inode *inode,struct file *filp)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.read = led_read,
	.write = led_write,
	.unlocked_ioctl = unlocked_led_ioctl,
	.release = led_release,
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

	led_setup_cdev(led_dev,0,&dev_fops);
	led_class = class_create(THIS_MODULE,DEVICE_NAME);
	if(IS_ERR(led_class))
	{
		printk(KERN_INFO"Create class error\n");
		return -1;
	}
	device_create(led_class,NULL,dev,NULL,DEVICE_NAME,MINOR(dev));

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
