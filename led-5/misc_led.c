#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
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

ssize_t led_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
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

static struct miscdevice misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};
static int __init led_init(void)
{
	int retval;
	
	retval = misc_register(&misc);

	return retval;
}

static void __exit led_exit(void)
{
	misc_deregister(&misc);
}

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

module_init(led_init);
module_exit(led_exit);
