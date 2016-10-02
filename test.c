#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_world( void )
{
  printk( "hello world!\n" );
  return 0;
}

static void __exit goodbye_world( void )
{
  printk( "goodbye world!\n" );
}

module_init( hello_world );
module_exit( goodbye_world );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Sample test");
MODULE_AUTHOR("Geo Joseph");
