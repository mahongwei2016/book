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

struct svled_dev{
	struct miscdevice key_miscdev;
	unsigned char mem[SVLED_SIZE];
};

struct svled_dev *svled_devp;

static int svled_open(struct inode *inode,struct file *filp)
{
	unsigned int data;
	filp->private_data=iminor(inode)-SVLED_MINOR_START;
	return 0;
}

static int svled_release(struct inode *inode,struct file *filp)
{
	return 0;
}

static long svled_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	unsigned int num=filp->private_data;
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

static ssize_t svled_read(struct file *filp,char __user *buf,size_t size,loff_t *ppos)
{
	unsigned long p=*ppos;
	unsigned int count=size;
	int ret=0;
	struct svled_dev *dev=(struct svled_dev *)(svled_devp+(int)(filp->private_data));
	if(p>=SVLED_SIZE)
		return 0;
	if(count>SVLED_SIZE-p)
		count=SVLED_SIZE-p;
	if(copy_to_user(buf,dev->mem+p,count)){
		ret=-EFAULT;
	}else{
		*ppos+=count;
		ret=count;
		printk(KERN_INFO"read %u byte(s) from %lu\n",count,p);
	}
	return ret;
}

static ssize_t svled_write(struct file *filp,const char __user *buf,size_t size,loff_t *ppos)
{
	unsigned long p=*ppos;
	unsigned int count=size;
	int ret=0;
	struct svled_dev *dev=(struct svled_dev *)(svled_devp+(int)(filp->private_data));
	if(p>=SVLED_SIZE)
		return 0;
	if(count>SVLED_SIZE-p)
		count=SVLED_SIZE-p;
	if(copy_from_user(dev->mem+p,buf,count))
		ret=-EFAULT;
	else{
		*ppos+=count;
		ret=count;
		printk(KERN_INFO"written %u byte(s) from %lu\n",count,p);
	}
	return ret;
}

static loff_t svled_llseek(struct file *filp,loff_t offset,int orig)
{
	loff_t ret=0;
	switch(orig){
	case 0:
		if(offset<0){
			return -EINVAL;
			break;
		}
		if((unsigned int)offset>SVLED_SIZE){
			ret=-EINVAL;
			break;
		}
		filp->f_pos=(unsigned int)offset;
		ret=filp->f_pos;
		break;
	case 1:
		if((filp->f_pos+offset)>SVLED_SIZE){
			ret=-EINVAL;
			break;
		}
		if((filp->f_pos+offset)<0){
			ret=-EINVAL;
			break;
		}
		filp->f_pos+=offset;
		ret=filp->f_pos;
		break;
	}
	return ret;
}

static const struct file_operations svled_fops={
	.owner	=	THIS_MODULE,
	.llseek	=	svled_llseek,
	.write	=	svled_write,
	.read	=	svled_read,
	.unlocked_ioctl	=		svled_ioctl,
	.open	=	svled_open,
	.release	=	svled_release,
};

static void svled_setup_cdev(struct svled_dev *dev,int index)
{

	led_iomux[index] = ioremap(LEDIOMUX+4*index,4);
    writel(0b0101,led_iomux[index]);
	dev->key_miscdev.minor=SVLED_MINOR_START+index;
	sprintf(buff[index],"svled%d",index);
	dev->key_miscdev.name=(char*)(buff[index]);
	dev->key_miscdev.fops=&svled_fops;
	misc_register(&(dev->key_miscdev));
}

static int __init svled_init(void)
{
	int ret,i;
	unsigned int gdir,data;
	svled_devp = kzalloc(sizeof(struct svled_dev)*DEVICE_NUM,GFP_KERNEL);
	if(!svled_devp){
		ret=-ENOMEM;
		goto fail_malloc;
	}
	for(i=0;i<DEVICE_NUM;i++)
		svled_setup_cdev(svled_devp+i,i);
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
		misc_deregister(&((svled_devp+i)->key_miscdev));
		iounmap((void *)led_iomux[i]);
	}
	iounmap((void *)led_iogdir);
	iounmap((void *)led_data);
	kfree(svled_devp);
}
module_exit(svled_exit);

MODULE_AUTHOR("mahongwei");
MODULE_LICENSE("GPL v2");
