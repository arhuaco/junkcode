#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
  printk(KERN_INFO "Hola mundo \n");
  return 0;
}

static void hello_exit(void)
{
  printk(KERN_INFO "Adios, mundo cruel\n");
}

module_init(hello_init);
module_exit(hello_exit);
