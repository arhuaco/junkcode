/* Check the chapter 10 of Linux Device Drivers, third edition */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h> /* udelay */
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include <asm/io.h> /* outb */

#define NPORTS 1

static int major = 0;	/* dinámico */

unsigned long par_base = 0x378;
volatile int par_irq = 7;

MODULE_LICENSE("Dual BSD/GPL");

int
par_open (struct inode *inode, struct file *filp)
{
	return 0;
}


int
par_release (struct inode *inode, struct file *filp)
{
  return 0;
}

ssize_t
par_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return count;
}

ssize_t
par_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
  return count;
}


struct file_operations par_fops = {
	.owner	 = THIS_MODULE,
	.read	 = par_read,
	.write	 = par_write,
	.open	 = par_open,
	.release = par_release,
};

irqreturn_t
par_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
        printk(KERN_ALERT "Interrupt\n");

	return IRQ_HANDLED;
}


int par_init(void)
{
  int result;


  if (!request_region(par_base, NPORTS, "par")) /* puertos */
  {
    printk(KERN_INFO "par: No pude obtener dirección del puerto de I/O 0x%lx\n", par_base);
    return -ENODEV;
  }

  result = register_chrdev(major, "par", &par_fops);

  if (result < 0) /* Dinámico */
  {
    printk(KERN_INFO "par: can't get major number\n");
    release_region(par_base, NPORTS); 
    return result;
  }

  if (major == 0)
    major = result;

   par_irq = 7;

   result = request_irq(par_irq, par_interrupt, SA_INTERRUPT, "par", NULL);

   if (result)
   {
     printk(KERN_INFO "par: No pude obtener la IRQ%i\n", par_irq);
     par_irq = -1;
   }
   else
   { 
     outb_p(0x10, par_base+2);
     outb_p(0xFF, par_base);
     barrier();
     udelay(5);
   }

  return 0;
}

void par_cleanup(void)
{
  if (par_irq >= 0)
  {
    outb(0x0, par_base + 2); 
    free_irq(par_irq, NULL);

    unregister_chrdev(major, "par");
    release_region(par_base, NPORTS);
  }
}

module_init(par_init);
module_exit(par_cleanup);
