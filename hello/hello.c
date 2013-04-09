#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Houstar");
MODULE_DESCRIPTION("Hello World Module");
MODULE_ALIAS("a simplest module");

static int hello_init(void)
{
	printk(KERN_ALERT "Hello World\n");
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye World\n");
}

module_init(hello_init);
module_exit(hello_exit);

