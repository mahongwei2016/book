#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
MODULE_AUTHOR("mhw");
MODULE_LICENSE("GPL");
extern struct device my_bus;
extern struct bus_type my_bus_type;

static void my_dev_release(struct device *dev)
{
	
}
static ssize_t mydev_show(struct device *dev,char* buf)
{
	return sprintf(buf,"%s\n","This is my device!");
}

struct device my_dev={
	.bus=&my_bus_type,
	.init_name="my_dev",
	.parent=&my_bus,
	.release=my_dev_release,
};

static DEVICE_ATTR(dev,S_IRUGO,mydev_show,NULL);
static int __init my_device_init(void)
{
	device_register(&my_dev);
	device_create_file(&my_dev,&dev_attr_dev);
	return 0;
}

static void __exit my_device_exit(void)
{
	device_unregister(&my_dev);
}
module_init(my_device_init);
module_exit(my_device_exit);
