#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");
extern add(int a,int b);
int a=3;
int b=0;
char *p;
module_param(a,int,S_IRUGO|S_IWUSR);
module_param(b,int,S_IRUGO|S_IWUSR);
module_param(p,charp,S_IRUGO|S_IWUSR);
static int hello_init()
{
	printk(KERN_WARNING"hello world\n");
	printk("a=%d\n",a);
	printk("b=%d\n",b);
	printk("p=%s\n",p);
	printk("a+b=%d\n",add(a,b));
	return 0;
}
static void hello_exit()
{
	printk(KERN_WARNING"hello exit!\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("MAHONGWEI-SV");
MODULE_DESCRIPTION("THIS is a demo module to printk hello world");
MODULE_VERSION("v1.0-2016.11.9");
