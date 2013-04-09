#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/fs.h> 
#include <asm/uaccess.h> 
#include <linux/miscdevice.h>
#include <linux/pci.h>
#include <mach/map.h> 
#include <mach/regs-gpio.h>
#include <mach/gpio-bank-m.h>
#include <plat/gpio-cfg.h>

#define LED_MAJOR 240


MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

int led_open(struct inode *node,struct file *filp)
{
	unsigned tmp;
	tmp = readl(S3C64XX_GPMCON); 
	tmp = (tmp & ~(0x7U<<1)) |(0x1U);
	writel(tmp,S3C64XX_GPMCON);
	printk("\n#####open######\n");
	return 0;
}

ssize_t led_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	printk("\n#####read######\n");
	return count;
}

ssize_t led_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	char wbuf[0];
	unsigned tmp;
	printk("\n#####write#####\n");
	copy_from_user(wbuf,buf,count);
	switch(wbuf[0])
	{
		case 0: //off
			tmp = readl(S3C64XX_GPMDAT);
			tmp |=(0xf);
			writel(tmp,S3C64XX_GPMDAT);
			break;
		case 1: //on
			tmp =readl(S3C64XX_GPMDAT);
			tmp &=(~(0xf));
			writel(tmp,S3C64XX_GPMDAT);
			break;
		default:
			break;
	}

	return count;
}

int led_release(struct inode *inode,struct file *filp)
{
	printk("\n#######release####\n");
	return 0;
}

const struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.read = led_read,
	.write = led_write,
	.release = led_release,
};

int __init led_init(void)
{
	int rc;
	printk("\nTest Led Device\n");
	rc = register_chrdev(LED_MAJOR,"led",&led_fops);
	if(rc < 0)
	{
		printk("register %s char_dev error\n","led");
		return -1;
	}
	printk("\nInitialing OK!\n");

	return 0;
}

void  __exit led_exit(void)
{
	unregister_chrdev(LED_MAJOR,"led");
	printk("\nmodule exit\n");
	return;
}

module_init(led_init);
module_exit(led_exit);

