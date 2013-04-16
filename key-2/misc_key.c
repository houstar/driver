#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/irq.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/delay.h>

#include <mach/map.h>
#include <mach/irqs.h>
//#include <mach/hardware.h>
#include <plat/gpio-cfg.h>


#define DEVICE_NAME "keys"
#define KEYNUM 6
#define MAX_KEY_COUNT 32
typedef struct _st_dev_info
{
	unsigned int irq_no; /* number of irq*/
	unsigned long flags; /*the way trigger irq*/
	unsigned char dev_id; /*device id*/
} st_dev_info;

typedef struct _st_dev_buffer
{
	unsigned long jiffy[MAX_KEY_COUNT];
	unsigned char buf[MAX_KEY_COUNT];
	unsigned int head,tail;
} st_dev_buffer;

static st_dev_info dev_info[KEYNUM] = 
{
	{IRQ_EINT(0),IRQF_TRIGGER_FALLING ,'1'},
	{IRQ_EINT(1),IRQF_TRIGGER_FALLING ,'2'},
	{IRQ_EINT(2),IRQF_TRIGGER_FALLING ,'3'},
	{IRQ_EINT(3),IRQF_TRIGGER_FALLING ,'4'},
	{IRQ_EINT(4),IRQF_TRIGGER_FALLING ,'5'},
	{IRQ_EINT(5),IRQF_TRIGGER_FALLING ,'6'},
};

static st_dev_buffer dev_buffer;
static spinlock_t buffer_lock;

static void init_gpio(void)
{
	int i;
	for(i =0;i < KEYNUM;i++)
	{
		s3c_gpio_cfgpin(S3C64XX_GPN(i),2);
		irq_set_irq_type(IRQ_EINT(i),dev_info[i].flags);
	}
}
static void init_devbuffer(void)
{
	int i;
	spin_lock_irq(&buffer_lock);
	for(i = 0;i < MAX_KEY_COUNT;i++)
	{
		dev_buffer.jiffy[i] = 0;
		dev_buffer.buf[i] =0;
	}
	dev_buffer.head = dev_buffer.tail = MAX_KEY_COUNT - 1;
	spin_unlock_irq(&buffer_lock);
}

static unsigned long get_tick_count(void)
{
	struct timeval currTick;
	unsigned long ulRet;
	do_gettimeofday(&currTick);
	ulRet = currTick.tv_sec;
	ulRet *=1000;
	ulRet +=(currTick.tv_usec + 500)/1000;
	return ulRet;
}

static __inline unsigned char dev_scan(int irq)
{
	unsigned char dev_id = 0xff;
	/* firstly,you should configure GPIO Input mode to detect
	 * the pin mode,if mode is lowlevel,detected what key you 
	 * pressed..
	 */
	int i=0;
	for(i = 0;i < KEYNUM;i++)
	{
		if(irq == dev_info[i].irq_no)
		{
			dev_id = dev_info[i].dev_id;
			return dev_id;
		}
	}
	return dev_id;

}

static __inline void free_irqs(void)
{
	int i;
	for(i = 0;i < KEYNUM;i++)
	{
		free_irq(dev_info[i].irq_no,(void *)&dev_info[i]);
	}
}

static __inline void disable_irqs(void)
{
	int i;
	for(i=0; i < KEYNUM;i++)
	{
		disable_irq(dev_info[i].irq_no);
	}
}

static __inline void enable_irqs(void)
{
	int i;
	for(i = 0; i<KEYNUM;i++)
	{
		enable_irq(dev_info[i].irq_no);
	}
}
static irqreturn_t dev_interrupt(int irq,void *dev_id)
{
	unsigned char ucKey = 0;

	disable_irqs();
	/* delay 50ms*/
	mdelay(50000);
	ucKey = dev_scan(irq);
	if((ucKey >=1) && (ucKey <=6))
	{
		if(((dev_buffer.head+1) & (MAX_KEY_COUNT -1)) != dev_buffer.tail)
		{
			spin_lock_irq(&buffer_lock);
			dev_buffer.buf[dev_buffer.tail] = ucKey;
			dev_buffer.jiffy[dev_buffer.tail] = get_tick_count();
			dev_buffer.tail++;
			dev_buffer.tail &= (MAX_KEY_COUNT -1);
			spin_unlock_irq(&buffer_lock);
		}
	}

	init_gpio();
	enable_irqs();

	return IRQ_RETVAL(IRQ_HANDLED);
}

static int request_irqs(void)
{
	int ret,i,j;
	for(i = 0;i < KEYNUM; i++)
	{
		ret = request_irq(dev_info[i].irq_no,dev_interrupt,dev_info[i].flags,&dev_info[i].dev_id,(void *)&dev_info[i]);
		if( ret < 0)
		{
			printk(KERN_INFO"error when register irq\n");
			printk(KERN_INFO"%x",ret);
			printk(KERN_INFO"%d",ret);
			for(j = 0; j < i; j++)
			{
				free_irq(dev_info[j].irq_no,(void *)&dev_info[i]);
			}
			return -EFAULT;
		}
	}
	return 0;
}

ssize_t dev_open(struct inode *inode,struct file *filp)
{
	int ret = nonseekable_open(inode,filp);
	if(ret < 0)
	{
		return ret;
	}
	init_gpio();
	ret = request_irqs();
	if(ret < 0)
	{
		return ret;
	}
	init_devbuffer();
	return ret;
}

static void remove_timeoutkey(void)
{
	unsigned long tick;
	spin_lock_irq(&buffer_lock);
	while(dev_buffer.head != dev_buffer.tail)
	{
		tick = get_tick_count() - dev_buffer.jiffy[dev_buffer.head];
		if(tick < 5000) break; 
		dev_buffer.buf[dev_buffer.head] = 0;
		dev_buffer.jiffy[dev_buffer.head] = 0;
		dev_buffer.head++;
		dev_buffer.head &= (MAX_KEY_COUNT -1);
	}
	spin_unlock_irq(&buffer_lock);
}

ssize_t dev_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
	ssize_t ret = 0;
	remove_timeoutkey();
	spin_lock_irq(&buffer_lock);
	while((dev_buffer.head != dev_buffer.tail) && (((size_t)ret < count)))
	{
		put_user((char)(dev_buffer.buf[dev_buffer.head]),&buf[ret]);
		dev_buffer.buf[dev_buffer.head] = 0;
		dev_buffer.jiffy[dev_buffer.head] = 0;
		dev_buffer.head++;
		dev_buffer.head &= (MAX_KEY_COUNT - 1);
		ret++;
	}
	spin_unlock_irq(&buffer_lock);
	return ret;
}

ssize_t dev_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	return 0;
}

static long unlocked_dev_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	return 0;
}

static int dev_release(struct inode *inode,struct file *filp)
{
	free_irqs();
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
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};
static int __init dev_init(void)
{
	int ret;
	spin_lock_init(&buffer_lock);
	ret = misc_register(&misc);
	return 0;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

MODULE_AUTHOR("Houstar<houstar@foxmail.com>");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);

