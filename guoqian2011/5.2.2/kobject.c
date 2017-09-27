#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>
struct attribute test_attr={
	.name="kobj_config",
	.mode=S_IRWXUGO,
};

struct attribute *def_attrs[]={
	&test_attr,
	NULL,	
};

ssize_t kobj_test_show(struct kobject *kobj,struct attribute *attr,char *buf)
{
	printk("have show.\n");
	printk("attrname:%s\n",attr->name);
	sprintf(buf,"%s\n",attr->name);
	return strlen(attr->name)+2;
}

ssize_t kobj_test_store(struct kobject *kobj,struct attribute *attr,const char *buf,size_t len)
{
	printk("have store\n");
	printk("write:%s\n",buf);
	return len;
}

struct sysfs_ops obj_test_sysops={
	.show=kobj_test_show,
	.store=kobj_test_store,
};

void obj_test_release(struct kobject *obj)
{
	printk("kobject_test:release.\n");
}

struct kobj_type ktype={
	.release=obj_test_release,
	.sysfs_ops=&obj_test_sysops,
	.default_attrs=def_attrs,
};

struct kobject kobj;
static int __init kobj_test_init(void)
{
	printk("kobject test init.\n");
	return kobject_init_and_add(&kobj,&ktype,NULL,"kobject_test");
}
module_init(kobj_test_init);

static void __exit kobj_test_exit(void)
{
	printk("kobject test exit.\n");
	kobject_del(&kobj);
}
module_exit(kobj_test_exit);

MODULE_AUTHOR("MHW");
MODULE_LICENSE("GPL");
