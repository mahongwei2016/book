#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "svled.h"

#define SVLED_SIZE 0x1000
#define SVLED_MINOR_START 220
#define DEVICE_NUM 7
char buff[DEVICE_NUM][100];

#define LEDIOMUX 0x20e0250
#define LEDIOGDIR 0x20b4004
#define LEDDAT 0x20b4000
unsigned int* led_iomux[DEVICE_NUM];
unsigned int *led_iogdir;
unsigned int *led_data;
struct miscdevice *led_miscdev;

static int svled_open(struct inode *inode,struct file *filp)
{
	filp->private_data=iminor(inode)-SVLED_MINOR_START;
	return 0;
}

static int svled_release(struct inode *inode,struct file *filp)
{
	return 0;
}

static long svled_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	unsigned int num=(unsigned int)(filp->private_data);
	unsigned int data;
	switch(cmd){
	case LED_ON:
		data=readl(led_data);data|=(1<<num);writel(data,led_data);
		break;
	case LED_OFF:
		data=readl(led_data);data&=~(1<<num);writel(data,led_data);
		break;
	default:
		return -EINVAL;
	}
	return 0;	
}

static const struct file_operations svled_fops={
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=		svled_ioctl,
	.open	=	svled_open,
	.release	=	svled_release,
};

static void svled_setup_cdev(struct miscdevice *dev,int index)
{

	led_iomux[index] = ioremap(LEDIOMUX+4*index,4);
    writel(0b0101,led_iomux[index]);
	dev->minor=SVLED_MINOR_START+index;
	sprintf(buff[index],"svled%d",index);
	dev->name=(char*)(buff[index]);
	dev->fops=&svled_fops;
	misc_register(dev);
}

static int __init svled_init(void)
{
	int ret,i;
	unsigned int gdir,data;
	led_miscdev = kzalloc(sizeof(struct miscdevice)*DEVICE_NUM,GFP_KERNEL);
	if(!led_miscdev){
		ret=-ENOMEM;
		goto fail_malloc;
	}
	for(i=0;i<DEVICE_NUM;i++)
		svled_setup_cdev(led_miscdev+i,i);
	led_iogdir = ioremap(LEDIOGDIR,4);
	gdir=readl(led_iogdir);
    gdir|=((1<<DEVICE_NUM)-1);
    writel(gdir,led_iogdir);
    led_data = ioremap(LEDDAT,4);
	data=readl(led_data);data&=~((1<<DEVICE_NUM)-1);writel(data,led_data); 
	return 0;
	
	fail_malloc:
		return ret;
}
module_init(svled_init);

static void __exit svled_exit(void)
{
	int i;
	for(i=0;i<DEVICE_NUM;i++)
	{
		misc_deregister(led_miscdev+i);
		iounmap((void *)led_iomux[i]);
	}
	iounmap((void *)led_iogdir);
	iounmap((void *)led_data);
	kfree(led_miscdev);
}
module_exit(svled_exit);

MODULE_AUTHOR("mahongwei");
MODULE_LICENSE("GPL v2");
