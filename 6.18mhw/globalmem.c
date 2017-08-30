#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MINOR_START 220
#define DEVICE_NUM 5
char buff[DEVICE_NUM][100];

struct globalmem_dev{
	struct miscdevice key_miscdev;
	unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_devp;

static int globalmem_open(struct inode *inode,struct file *filp)
{
	struct globalmem_dev *dev;
	dev=globalmem_devp+iminor(inode)-GLOBALMEM_MINOR_START;
	filp->private_data=dev;
	return 0;
}

static int globalmem_release(struct inode *inode,struct file *filp)
{
	return 0;
}

static long globalmem_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	struct globalmem_dev *dev=filp->private_data;
	switch(cmd){
	case MEM_CLEAR:
		memset(dev->mem,0,GLOBALMEM_SIZE);
		printk(KERN_INFO"globalmem is set to zero\n");
		break;
	default:
		return -EINVAL;
	}
	return 0;	
}

static ssize_t globalmem_read(struct file *filp,char __user *buf,size_t size,loff_t *ppos)
{
	unsigned long p=*ppos;
	unsigned int count=size;
	int ret=0;
	struct globalmem_dev *dev=filp->private_data;
	if(p>=GLOBALMEM_SIZE)
		return 0;
	if(count>GLOBALMEM_SIZE-p)
		count=GLOBALMEM_SIZE-p;
	if(copy_to_user(buf,dev->mem+p,count)){
		ret=-EFAULT;
	}else{
		*ppos+=count;
		ret=count;
		printk(KERN_INFO"read %u byte(s) from %lu\n",count,p);
	}
	return ret;
}

static ssize_t globalmem_write(struct file *filp,const char __user *buf,size_t size,loff_t *ppos)
{
	unsigned long p=*ppos;
	unsigned int count=size;
	int ret=0;
	struct globalmem_dev *dev=filp->private_data;
	if(p>=GLOBALMEM_SIZE)
		return 0;
	if(count>GLOBALMEM_SIZE-p)
		count=GLOBALMEM_SIZE-p;
	if(copy_from_user(dev->mem+p,buf,count))
		ret=-EFAULT;
	else{
		*ppos+=count;
		ret=count;
		printk(KERN_INFO"written %u byte(s) from %lu\n",count,p);
	}
	return ret;
}

static loff_t globalmem_llseek(struct file *filp,loff_t offset,int orig)
{
	loff_t ret=0;
	switch(orig){
	case 0:
		if(offset<0){
			return -EINVAL;
			break;
		}
		if((unsigned int)offset>GLOBALMEM_SIZE){
			ret=-EINVAL;
			break;
		}
		filp->f_pos=(unsigned int)offset;
		ret=filp->f_pos;
		break;
	case 1:
		if((filp->f_pos+offset)>GLOBALMEM_SIZE){
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

static const struct file_operations globalmem_fops={
	.owner	=	THIS_MODULE,
	.llseek	=	globalmem_llseek,
	.write	=	globalmem_write,
	.read	=	globalmem_read,
	.unlocked_ioctl	=		globalmem_ioctl,
	.open	=	globalmem_open,
	.release	=	globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev,int index)
{
	dev->key_miscdev.minor=GLOBALMEM_MINOR_START+index;
	sprintf(buff[index],"globalmem%d",index);
	dev->key_miscdev.name=(char*)(buff[index]);
	dev->key_miscdev.fops=&globalmem_fops;
	misc_register(&(dev->key_miscdev));
}

static int __init globalmem_init(void)
{
	int ret,i;
	globalmem_devp = kzalloc(sizeof(struct globalmem_dev)*DEVICE_NUM,GFP_KERNEL);
	if(!globalmem_devp){
		ret=-ENOMEM;
		goto fail_malloc;
	}
	for(i=0;i<DEVICE_NUM;i++)
		globalmem_setup_cdev(globalmem_devp+i,i);
	return 0;
	
	fail_malloc:
		return ret;
}
module_init(globalmem_init);

static void __exit globalmem_exit(void)
{
	int i;
	for(i=0;i<DEVICE_NUM;i++)
		misc_deregister(&((globalmem_devp+i)->key_miscdev));
	kfree(globalmem_devp);
}
module_exit(globalmem_exit);

MODULE_AUTHOR("mahongwei");
MODULE_LICENSE("GPL v2");
