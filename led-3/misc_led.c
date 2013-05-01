/*
 * misc_led.c
 */
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-bank-e.h>
#include <mach/gpio-bank-m.h>

#define DEVICE_NAME "leds"

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

static long leds_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
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

static struct file_operations led_fops = {
	.owner =THIS_MODULE,
	.unlocked_ioctl=leds_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &led_fops,
};

static int __init led_init(void)
{
	int ret;

	unsigned tmp;

	/*GPM0-3 pull up */
	tmp = readl(S3C64XX_GPMPUD);
	tmp &=(~0xff);
	tmp |= 0xaa;
	writel(tmp,S3C64XX_GPMPUD);

	/*GPM0-3 output mode*/
	tmp = readl(S3C64XX_GPMCON);
	tmp &=(~0xffff);
	tmp |= 0x1111;
	writel(tmp,S3C64XX_GPMCON);

	/* GPM0-3 output 1 */
	tmp = readl(S3C64XX_GPMDAT);
	tmp |=(~0x00);
	writel(tmp,S3C64XX_GPMDAT);

	ret = misc_register(&misc);

	return ret;
}

static void __exit led_exit(void)
{
	misc_deregister(&misc);
}

module_init(led_init);
module_exit(led_exit);
