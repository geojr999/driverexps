#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include<linux/slab.h>

#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)\
do { \
long timeout = (nMilliSec) * HZ / 1000; \
while(timeout > 0) \
{ \
timeout = schedule_timeout(timeout); \
} \
}while(0);
#endif

static dev_t first;        // Global variable for the first device number
static struct cdev c_dev;  // Global variable for the character device structure
static struct class *cl;   // Global variable for the device class

static int init_result;

static struct task_struct * MyThread = NULL;

static int MyPrintk(void *data)
{
char *mydata = kmalloc(strlen(data)+1,GFP_KERNEL);
memset(mydata,'\0',strlen(data)+1);
strncpy(mydata,data,strlen(data));
while(!kthread_should_stop())
{
SLEEP_MILLI_SEC(1000);
printk("%s\n",mydata);
}
kfree(mydata);
return 0;
}

static ssize_t dummy_read( struct file* F, char *buf, size_t count, loff_t *f_pos )
{
	unsigned long j, stamp_1, stamp_half, stamp_n;
	j = jiffies;                      /* read the current value */
	stamp_1    = j + HZ;              /* 1 second in the future */
	stamp_half = j + HZ/2;            /* half a second */
	stamp_n    = j + 5 * HZ / 1000;   /* n milliseconds */   	
	 printk("j %lu, stamp_1 %lu, stamp_half %lu, stamp_n %lu\n",j, stamp_1, stamp_half, stamp_n);

	return 0;
}

static ssize_t dummy_write( struct file* F, const char *buf, size_t count, loff_t *f_pos )
{
 
    printk(KERN_INFO "Executing WRITE.\n");
 
    switch( buf[0] )
    {
        case '0':
	MyThread = kthread_run(MyPrintk,"hello world","mythread"); 
   	printk("Executing WRITE.0\n");
        break;
 
        case '1':
   	if(MyThread)
	{
	printk("stop MyThread\n");
	kthread_stop(MyThread);
	}
	 printk("Executing WRITE. 1\n");
        break;
 
        default:
        printk("Wrong option.\n");
        break;
    }
    return count;
}



static int dummy_open( struct inode *inode, struct file *file )
{
    return 0;
}
 
static int dummy_close( struct inode *inode, struct file *file )
{
    return 0;
}

static struct file_operations FileOps =
{
    .owner        = THIS_MODULE,
    .open         = dummy_open,
    .read         = dummy_read,
    .write        = dummy_write,
    .release      = dummy_close,
};

static int __init hello_world( void )
{
 
    init_result = alloc_chrdev_region( &first, 0, 1, "dummy_drv" );
 
    if( 0 > init_result )
    {
        printk( KERN_ALERT "Device Registration failed\n" );
        return -1;
    }
    //else
    //{
    //    printk( KERN_ALERT "Major number is: %d\n",init_result );
    //    return 0;
    //}
 
    if ( (cl = class_create( THIS_MODULE, "chardev" ) ) == NULL )
    {
        printk( KERN_ALERT "Class creation failed\n" );
        unregister_chrdev_region( first, 1 );
        return -1;
    }
 
    if( device_create( cl, NULL, first, NULL, "dummy_drv" ) == NULL )
    {
        printk( KERN_ALERT "Device creation failed\n" );
        class_destroy(cl);
        unregister_chrdev_region( first, 1 );
        return -1;
    }
 
    cdev_init( &c_dev, &FileOps );
 
    if( cdev_add( &c_dev, first, 1 ) == -1)
    {
        printk( KERN_ALERT "Device addition failed\n" );
        device_destroy( cl, first );
        class_destroy( cl );
        unregister_chrdev_region( first, 1 );
        return -1;
    }
 
    return 0;

//  printk( "hello world!\n" );
//  return 0;
}

static void __exit goodbye_world( void )
{
	cdev_del( &c_dev );
    	device_destroy( cl, first );
    	class_destroy( cl );
    	unregister_chrdev_region( first, 1 );
 
    printk(KERN_ALERT "Device unregistered\n"); 
}

module_init( hello_world );
module_exit( goodbye_world );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sample test");
MODULE_AUTHOR("Geo Joseph");
