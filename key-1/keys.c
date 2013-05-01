#include<linux/fs.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/platform_device.h>
#include<linux/types.h>
#include<linux/device.h>
#include<linux/sched.h>
#include<mach/irqs.h>
#include<asm/irq.h>
#include<linux/irq.h>
#include<mach/hardware.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#define key_major 237
#define key_minor 0
 
dev_t dev_num;
struct cdev *keyp;
struct class *key_class;
#define DEVICE_NAME "keyint"
volatile int press_cnt[]={0,0,0,0,0,0};
volatile bool ev_press=0;
DECLARE_WAIT_QUEUE_HEAD(key_wait);

struct irq_key_desc{
unsigned int irq;//中断号
unsigned int flags;//标致
const char *dev_name;//按键名字
};

struct irq_key_desc   key_irq[]={
{IRQ_EINT(0),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key1"},
{IRQ_EINT(1),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key2"},
{IRQ_EINT(2),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key3"},
{IRQ_EINT(3),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key4"},
{IRQ_EINT(4),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key5"},
{IRQ_EINT(5),IRQF_TRIGGER_FALLING|IRQF_SHARED,"key6"},
};

irqreturn_t key_interrupt(int irq,void *dev_id){
	volatile int *press_cnt=(volatile int *)dev_id;
	*press_cnt=*press_cnt+1;
    ev_press=1;
	wake_up_interruptible(&key_wait);	
    printk(KERN_INFO"中断处理结束");
	return IRQ_RETVAL(IRQ_HANDLED);
}	

int key_open(struct inode *inode,struct file *filp){
	int i;
   	int err;
    for(i=0;i<6;i++){
    err=request_irq(key_irq[i].irq,key_interrupt,key_irq[i].flags,key_irq[i].dev_name,(void *)&press_cnt[i]);
    if(err<0)
	printk(KERN_INFO"注册中断出错 \n");
    printk(KERN_INFO"%x",err); 
    } 
    printk(KERN_INFO"%d",err); 
	return 0; 
 }
int key_close(struct inode *inode,struct file *filp){
    int i;
  	for(i=0;i<6;i++)
    free_irq(key_irq[i].irq,(void *)&press_cnt[i]);
	return 0;
}  

ssize_t key_read(struct file *filp, char __user *buf,size_t count,loff_t *offp){
    printk(KERN_INFO"开始读");
    wait_event_interruptible(key_wait,ev_press);
    ev_press=0;
    copy_to_user(buf,(const void *)press_cnt,count);
    memset((void *)press_cnt,0,sizeof(press_cnt));
 	return 1;
} 

struct file_operations key_ops ={
	.owner=THIS_MODULE,
 	.open=key_open,
	.release=key_close,
	.read=key_read,
};  

void key_cdev_setup(void){
	int err;
	cdev_init(keyp,&key_ops);
	keyp->owner=THIS_MODULE;
	keyp->ops=&key_ops;
	err=cdev_add(keyp,dev_num,1);
    
}    

static int __init  keyp_init(void)  
{
	int ret;
	dev_num=MKDEV(key_major,key_minor);
	if(key_major)
 	ret=register_chrdev_region(dev_num,1,DEVICE_NAME);
	else ret=alloc_chrdev_region(&dev_num,key_minor,1,DEVICE_NAME);
	if(ret<0)
   {printk(KERN_INFO"申请设备号失败");  return 0;}
  	keyp=kmalloc(sizeof(struct cdev),GFP_KERNEL);
	printk(KERN_INFO"地址 %o \n",keyp);
	memset(keyp,0,sizeof(struct cdev));
	key_cdev_setup();
	printk(KERN_INFO"cdev创建成功");
	key_class=class_create(THIS_MODULE,DEVICE_NAME);
	device_create(key_class,NULL,dev_num,NULL,DEVICE_NAME);
	return 0;
}   

static int __exit keyp_exit(void)
{ 
 	cdev_del(keyp);
	kfree(keyp);
    
	unregister_chrdev_region(dev_num,1);
    return 0;
}   
module_init(keyp_init);
module_exit(keyp_exit);
MODULE_LICENSE("GPL");

