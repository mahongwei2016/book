#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>


static struct platform_device globalfifo_device={
	.name="globalfifo",
	.id=-1,	
};

static int __init globalfifo_init(void)
{
	int ret=platform_device_register(&globalfifo_device);
	return ret;
}
module_init(globalfifo_init);

static void __exit globalfifo_exit(void)
{
	platform_device_unregister(&globalfifo_device);
}
module_exit(globalfifo_exit);

MODULE_AUTHOR("mahongwei");
MODULE_LICENSE("GPL v2");
