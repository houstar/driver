#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/io.h>
//#include <asm/hardware.h>
#include <linux/string.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank-m.h>


#define LED_MAJOR 250
#define LED_ON 1
#define LED_OFF 0

MODULE_AUTHOR("yixuaning <yixuaning@sina.com>");
MODULE_LICENSE("Dual BSD/GPL");

int led_major=LED_MAJOR;

struct LED_DEV{
	struct cdev cdev;
	char value;
};

void led_on(void)
{
	__raw_writel((__raw_readl(S3C64XX_GPMDAT)&0x0),S3C64XX_GPMDAT);
}

void led_off(void)
{
	__raw_writel((__raw_readl(S3C64XX_GPMDAT)|0xF),S3C64XX_GPMDAT);
}

int  led_open(struct inode* inode,struct file* filp)
{
	struct LED_DEV* dev;
	dev=container_of(inode->i_cdev,struct LED_DEV,cdev);
	filp->private_data=dev;
	return 0;
}

int  led_release(struct inode* inode,struct file* filp)
{
	return 0;
}
/*ssize_t led_read(struct file* filp,char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

ssize_t led_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}*/

long led_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	struct LED_DEV *led_dev=filp->private_data;
	switch(cmd){
	case LED_ON:{
		led_dev->value=1;
		led_on();
		break;
	}
	case LED_OFF:{
		led_dev->value=0;
		led_off();
		break;	
	}
	default:
		return -ENOTTY;
	}
	return 0;
}

struct file_operations led_fops={
	.owner=THIS_MODULE,
	.open=led_open,
	//.read=led_read,
	//.write=led_write,
	.unlocked_ioctl=led_ioctl,

	.release=led_release,
};

struct LED_DEV* led_dev;

static int __init led_init(void)
{
	int result;
	dev_t devno;
	if(LED_MAJOR){
		devno=MKDEV(LED_MAJOR,0);
		result=register_chrdev_region(devno,1,"led_dev");
	}else{
		result=alloc_chrdev_region(&devno,0,1,"led_dev");
		led_major=MAJOR(devno);
	}
	if(result<0){
		printk(KERN_WARNING "ERROR: can not register\n");
		return result;
	}
	
	led_dev=kmalloc(sizeof(struct LED_DEV),GFP_KERNEL);
	if(!led_dev){
		result=-ENOMEM;
		goto fail;
	}
	memset(led_dev,0,sizeof(struct LED_DEV));
	cdev_init(&led_dev->cdev,&led_fops);
	led_dev->cdev.owner=THIS_MODULE;
	result=cdev_add(&led_dev->cdev,devno,1);
	if(result){
		printk(KERN_WARNING "ERROR: can not add cdev\n");
		goto fail;
	}
	__raw_writel((__raw_readl(S3C64XX_GPMCON)|0x1111),S3C64XX_GPMCON);
	__raw_writel((__raw_readl(S3C64XX_GPMDAT)|0xF),S3C64XX_GPMDAT);
	__raw_writel((__raw_readl(S3C64XX_GPMPUD)&0x0),S3C64XX_GPMPUD);
	return 0;
fail:
	unregister_chrdev_region(devno,1);
	return result;
}

static void __exit led_exit(void)
{
	dev_t devno=MKDEV(led_major,0);
	if(led_dev){
		cdev_del(&led_dev->cdev);
		kfree(led_dev);
	}
	unregister_chrdev_region(devno,1);
}

module_init(led_init);
module_exit(led_exit);
