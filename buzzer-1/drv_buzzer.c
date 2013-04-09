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
#include <mach/gpio-bank-f.h>

#define DEVICE_NAME "buzzer"

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

static long buzzer_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case 0:
		case 1:
			if(cmd)//buzzer_on
				__raw_writel(__raw_readl(S3C64XX_GPFDAT)|(1<<15),S3C64XX_GPFDAT);
			else // buzzer_off
				__raw_writel(__raw_readl(S3C64XX_GPFDAT)&(~(1<<15)),S3C64XX_GPFDAT);
			return 0;
		default:
			return -EINVAL;
	}
}

static struct file_operations buzzer_fops = {
	.owner =THIS_MODULE,
	.unlocked_ioctl=buzzer_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &buzzer_fops,
};

static int __init buzzer_init(void)
{
	int ret;

	unsigned tmp;

	/*GPM15[31:30] = 01 output mode*/
	tmp = readl(S3C64XX_GPFCON);
	tmp &=(~0x3<<30);
	tmp |=(0x1<<30);
	writel(tmp,S3C64XX_GPFCON);

	/*GPM15 output 0*/
	tmp = readl(S3C64XX_GPFDAT);
	tmp &=(~(1<<15));
	writel(tmp,S3C64XX_GPFDAT);

	ret = misc_register(&misc);

	return ret;
}

static void __exit buzzer_exit(void)
{
	misc_deregister(&misc);
}

module_init(buzzer_init);
module_exit(buzzer_exit);
