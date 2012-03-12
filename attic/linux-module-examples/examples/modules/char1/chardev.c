#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* get_user and put_user */

#define SUCCESS 0
#define DEVICE_FILE_NAME "char_dev"
#define DEVICE_NAME "char_dev"
#define MAJOR_NUM 100
#define BUF_LEN 80

#define DEBUG 

static atomic_t Device_Open = ATOMIC_INIT(1);

static char Message[BUF_LEN]; /* Buffer */
static char *Message_Ptr;     /* ¿Qué tan lejos llegó la lectura? */

MODULE_LICENSE("Dual BSD/GPL");

static int device_open(struct inode *inode, struct file *file)
{

#ifdef DEBUG
  printk(KERN_INFO "device_open(%p)\n", file);
#endif

  if (!atomic_dec_and_test (&Device_Open))
  {
    atomic_inc(&Device_Open);
    return -EBUSY; /* ya está abierto */
  }

  Message_Ptr = Message;
  try_module_get(THIS_MODULE);

  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
  printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

  atomic_inc(&Device_Open);

  module_put(THIS_MODULE);
  return SUCCESS;
}

static ssize_t device_read(struct file *file,    /* ver include/linux/fs.h   */
                           char __user * buffer, /* buffer a llenar con datos */
                           size_t length,        /* longitud del buffer  */
                           loff_t * offset)
{
  int bytes_read = 0; /* bytes escritos en el buffer */

#ifdef DEBUG
  printk(KERN_INFO "device_read(%p,%p,%d)\n", file, buffer, length);
#endif

  if (*Message_Ptr == 0) /* retornar EOF */
    return 0;

  while (length && *Message_Ptr)
  {
    put_user(*(Message_Ptr++), buffer++); /* escribe al buffer del usuario */
    length--;
    bytes_read++;
  }

#ifdef DEBUG
  printk(KERN_INFO "Lei %d bytes, quedan %d\n", bytes_read, length);
#endif

  return bytes_read; /* bytes escritos al buffer */
}

static ssize_t
device_write(struct file *file,
             const char __user * buffer, size_t length, loff_t * offset)
{
  int i;

#ifdef DEBUG
  printk(KERN_INFO "device_write(%p,%p,%d)", file, buffer, length);
#endif

  for (i = 0; i < length && i < BUF_LEN; i++)
    get_user(Message[i], buffer + i);

  Message_Ptr = Message;

  return i;
}

struct file_operations Fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release,
};

int init_module()
{
  int ret_val;

  ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

  if (ret_val < 0)
  {
    printk(KERN_ALERT "El registro del dispositivo falló (%d)\n", ret_val);
    return ret_val;
  }

  printk(KERN_INFO "Se registró el dispositivo. El major device number es %d.\n", MAJOR_NUM);
  printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);

return 0;
}

void cleanup_module()
{
  int ret;

  ret = unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

  if (ret < 0)
    printk(KERN_ALERT "Error: unregister_chrdev: %d\n", ret);
}
