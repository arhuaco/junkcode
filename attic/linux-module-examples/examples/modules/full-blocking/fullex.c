#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>
#include <linux/cdev.h>         /* char devices */
#include <linux/errno.h>        /* códigos de error */
#include <asm/uaccess.h>        /* get_user y put_user */

#include "fullex.h" /* definiciones para ioctl */

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Nelson Castillo"); 

#define SUCCESS 0
#define DEVICE_NAME "fullex"

#define DEBUG  /* Definir para depurar */

static struct fullex_dev
{
  char   *buff;             /* nuestro buffer, por dispositivo */
  int    buff_size;
  int    written;           /* cuantos bytes hemos escrito */

  wait_queue_head_t readq;  /* cola de procesos que están leyendo */

  struct semaphore sem;     /* exclusión mutua */

  struct cdev cdev;         /* el char device */
} *fullex_devices;          /* inicializado en init_module */

/* Funciones que operan sobre los datos privados de fullex_dev */

static int   fullex_kalloc(struct fullex_dev *dev, int size, int do_zero);
static void  fullex_reset(struct fullex_dev *dev);
static void  fullex_kfree(struct fullex_dev *dev);
static int   fullex_resize(struct fullex_dev *dev, int new_size);

/* Parámetros que se pueden enviar al módulo al insertarlo
 * En este caso, estamos enviando el major number. Si no se
 * especifica el major number, el módulo usará uno que se pide
 * al kernel dinámicamente
 *
 * Ejemplo:
 *
 * # insmod fullex.ko major=100 ndevs=5
 *
 * */

static int major       = 0;    /* dinámico por defecto */
static int ndevs       = 4;    /* número de devices a registrar */
static int buffer_size = 5;   /* Tamaño del buffer */

module_param(major, int, 0);
module_param(ndevs, int, 0);
module_param(buffer_size, int, 0);

static int device_open(struct inode *inode, struct file *filep)
{
  struct fullex_dev *dev;
  int ret_val = SUCCESS;

  /*
   * Busca la información privada de dispositivo.
   * container_of - hace el cast del miembro de la estructura a la estructura indicada
   */

  dev = container_of(inode->i_cdev, struct fullex_dev, cdev);

  if (down_interruptible (&dev->sem))
    return -ERESTARTSYS;

  nonseekable_open(inode, filep);

#ifdef DEBUG
  printk(KERN_INFO "device_open(file=%p, dev=%p)\n", filep, dev);
#endif

  if ((filep->f_flags & O_ACCMODE) == O_WRONLY)
    fullex_reset(dev); /* Se inicializa si se abre en modo sólo escritura */

  /* se usa filp->private_data para los datos del device abierto */
  filep->private_data = dev;

  up (&dev->sem);
  return ret_val;
}

static int device_release(struct inode *inode, struct file *filep)
{
  struct fullex_dev *dev = filep->private_data;

#ifdef DEBUG
  printk(KERN_INFO "device_release(dev=%p,inode=%p,file=%p)\n", dev, inode, filep);
#endif

  return SUCCESS;
}

static ssize_t device_read(struct file *filep,    /* ver include/linux/fs.h   */
                           char __user *buffer,   /* buffer a llenar con datos */
                           size_t length,         /* longitud del buffer  */
                           loff_t * offset)
{
  struct fullex_dev *dev = filep->private_data;
  long pos = (long)*offset;
  int ret_val;

#ifdef DEBUG
  printk(KERN_INFO "device_read(priv=%p,filep=%p,buff=%p,len=%Zd, offset=%ld)\n",
                   filep->private_data, filep, buffer, length, pos);
#endif

  if (down_interruptible (&dev->sem))
    return -ERESTARTSYS;

  if (pos >= dev->buff_size) /* retornar EOF */
  {
    ret_val = 0;
    goto read_exit;
  }

  /* TODO, el offset puede cambiar si se hace un ioctl */

  while (dev->buff[pos] == 0) /* no hemos llegado a EOF, pero todavía falta que llenen el device */
  {  
    up(&dev->sem); /* Soltar el lock! */

    /* Retornar enseguida se la llamada es no-bloqueante */

    if (filep->f_flags & O_NONBLOCK)
      return -EAGAIN;

#ifdef DEBUG
    printk(KERN_INFO "durmiendo en device_read, pos=%ld\n", pos);
#endif

    if (wait_event_interruptible(dev->readq, (dev->buff[pos] != 0)))
      return -ERESTARTSYS; /* se recibió una señal, dejemos que la maneje la capa fs */
 
    /* Sino, sigamos el loop. */
    if (down_interruptible(&dev->sem))
      return -ERESTARTSYS;
  }

  if (pos + length > dev->buff_size)
    length = dev->buff_size - pos;

  if (pos + length > dev->written)
    length = dev->written - pos;

  if (copy_to_user(buffer, dev->buff + pos, length))
  {
    ret_val = -EFAULT;
    goto read_exit;
  }

  *offset += length;
  ret_val = length;

#ifdef DEBUG
  printk(KERN_INFO "read (nbytes=%Zd, priv=%p)\n", length, dev);
#endif

read_exit:

  up(&dev->sem);

  return ret_val; /* bytes escritos al buffer */
}

static ssize_t
device_write(struct file *filep,
             const char __user *buffer, size_t length, loff_t *offset)
{
  struct fullex_dev *dev = filep->private_data;
  long pos = (long)*offset;
  int ret_val;

#ifdef DEBUG
  printk(KERN_INFO "device_write(filep=%p, buff=%p, len=%Zd, offset=%ld)\n",
                   filep, buffer, length, pos);
#endif

  if (down_interruptible (&dev->sem))
    return -ERESTARTSYS;

  if (pos >= dev->buff_size)
  {
    ret_val = EFBIG;
    goto write_exit;
  }

  if (length + pos > dev->buff_size)
    length = dev->buff_size - pos;

  if(copy_from_user(dev->buff + pos,  buffer, length))
  {
    ret_val = -EFAULT;
    goto write_exit;
  }
 
  dev->written += length;
  *offset += length;
  ret_val = length;
  
write_exit:

  up(&dev->sem);

  wake_up_interruptible(&dev->readq);

  return ret_val;
}

static int
fullex_ioctl(struct inode *inode, struct file *filp,
            unsigned int cmd, unsigned long arg)
{
  int err = 0;
  int ret_val;

  struct fullex_dev *dev = filp->private_data;

  if (down_interruptible (&dev->sem))
    return -ERESTARTSYS; 

#ifdef DEBUG
    printk(KERN_ALERT "ioctl %d (inode=%p,file=%p,cmd=%u,arg=%lu)\n",
           _IOC_NR(cmd), inode, filp, cmd, arg);
#endif

   /*
    * Extrae los bits de tipo de IOCTL y el número de la IOCTL.
    * La idea es no decodificar los llamados malos.
    * Retornar ENOTTY (ioctl no apropiado) antes de access_ok()
    */

  if (_IOC_TYPE(cmd) != FULLEX_IOC_MAGIC || _IOC_NR(cmd) > FULLEX_IOC_MAXNR)
  {
    ret_val = -ENOTTY;
    goto fullex_ioctl_exit;
  }

   /* Verifica el tipo de acceso, desde el punto de vista del usuario
    * No del kernel */

  if (_IOC_DIR(cmd) & _IOC_READ)
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
    err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));

  if (err)
  {
    ret_val = -EFAULT;
    goto fullex_ioctl_exit;
  }

  switch(cmd)
  {
    int new_size;

    case FULLEX_RESET:

      memset(dev->buff, dev->buff_size, 0);

      ret_val = SUCCESS;

      break;

    case FULLEX_SETSIZE:   /* SET: arg apunta al valor */

      if (!capable (CAP_SYS_ADMIN))
      {
        ret_val = -EPERM;
        goto fullex_ioctl_exit;
      }

      ret_val = __get_user(new_size, (int __user *)arg);

      if (!ret_val && new_size != dev->buff_size)
        ret_val = fullex_resize(dev, new_size);

      break;

    case FULLEX_GETSIZE: /* GET: arg es un puntero al resultado */

      ret_val = __put_user(dev->buff_size, (int __user *)arg);

      break;

    default:  /* redundante, ya revisamos cmd contra MAXNR */
      ret_val = -ENOTTY;
   }

fullex_ioctl_exit:
  
  up (&dev->sem);
  return ret_val;
}


static struct file_operations fullex_file_operations = {
  .owner =     THIS_MODULE,

  .open     = device_open,
  .release  = device_release,

  .read     = device_read,
  .write    = device_write,

  .llseek   = no_llseek,

  .ioctl    = fullex_ioctl,
};

static void fullex_setup_cdev(struct fullex_dev *dev, int index)
{
  int ret_value;

  /* Ver http://lwn.net/images/pdf/LDD3/ch03.pdf , página 55 */

  cdev_init(&dev->cdev, &fullex_file_operations); /* inicializa el char device */
  dev->cdev.owner = THIS_MODULE;

  /* registra el char device, con 1 minor number asociado */
  ret_value = cdev_add(&dev->cdev, MKDEV(major, index), 1);

  if (ret_value)
    printk(KERN_NOTICE "Error %d adding %s%d\n", ret_value, DEVICE_NAME, index);
}

/* Aparta memoria para un buffer, y la inicializa a ceros.
 * Retorna 0 si no se puede apartar la memoria */

static int fullex_kalloc(struct fullex_dev *dev, int size, int do_zero)
{
  dev->buff      = kmalloc(size, GFP_KERNEL);
  dev->buff_size = size;

  if (dev->buff && do_zero)
    memset(dev->buff, 0, size);

  return dev->buff != NULL;
}

void fullex_reset(struct fullex_dev *dev)
{
#ifdef DEBUG
  printk(KERN_INFO "fullex_reset(%p)\n", dev);
#endif

  memset(dev->buff, dev->buff_size, 0);
  dev->written = 0;
}

static void fullex_kfree(struct fullex_dev *dev)
{
  if (dev->buff)
    kfree(dev->buff);
}

/* Cambia el tamaÃo de buffer, no se pierde el contenido
 * anterior */

static int fullex_resize(struct fullex_dev *dev, int new_size)
{
  char *oldbuff    = dev->buff;
  int oldsize      = dev->buff_size;

  if (new_size <= 0)
    return -EINVAL;

  if (!fullex_kalloc(dev, new_size, 0)) /* no se inicializa a 0s en fullex_kalloc */
  {
    dev->buff       = oldbuff;
    dev->buff_size  = oldsize;

    return -ENOMEM;
  }

  memcpy(dev->buff, oldbuff, min(oldsize, dev->buff_size));
  kfree(oldbuff);

  return SUCCESS;
}

int init_module()
{
  int ret_val;
  int i;
  dev_t dev;
  
  dev = MKDEV(major, 0);

  if (major)
    ret_val = register_chrdev_region(dev, ndevs, DEVICE_NAME);
  else
  {
    /* usamos un major number dinámico */
    ret_val = alloc_chrdev_region(&dev, 0, ndevs, DEVICE_NAME);
    major = MAJOR(dev);
  }

  if (ret_val < 0)
  {
    printk(KERN_ALERT "Error registering %s: %d\n", DEVICE_NAME, ret_val);
    return ret_val;
  }

 /* Aparta memoria para ndevs dispositivos
  *
  * GFP_KERNEL: se aparta memoria y la llamada a kmalloc puede ser bloqueante.
  *
  * http://lwn.net/Articles/22909/ -- Driver porting: low-level memory allocation
  *
  */

  fullex_devices = kmalloc(ndevs * sizeof(struct fullex_dev), GFP_KERNEL);

  if (!fullex_devices)
  {
    ret_val = -ENOMEM;
    goto init_fail_malloc;
  }

 /* Inicializa a 0s
  * Es importante hacer esto después de un kmalloc exitoso */

  memset(fullex_devices, 0, ndevs * sizeof(struct fullex_dev));

  for (i = 0; i < ndevs; i++)
  {
    /* inicializamos datos privados del char device */

    if(!fullex_kalloc(fullex_devices + i, buffer_size, 1))
    {
      ret_val = -ENOMEM;
      goto init_fail_malloc;
    }

    sema_init(&fullex_devices[i].sem, 1);  /* inicializamos el semáforo */

    /* Inicializa colas de lectura y escritura */
    init_waitqueue_head(&(fullex_devices[i].readq));

    fullex_setup_cdev(fullex_devices + i, i);

    fullex_reset(fullex_devices + i);
  }

  printk(KERN_INFO "Device %s registration OK\n", DEVICE_NAME);

  for (i = 0; i < ndevs; i++)
    printk(KERN_INFO "mknod /dev/%s%d c %d %d\n", DEVICE_NAME, i, major, i);

  return 0;

  init_fail_malloc: /* Salida de la inicialización si un kmalloc falla */

    unregister_chrdev_region(dev, ndevs);

    for (i = 0; fullex_devices && i < ndevs; i++)
      fullex_kfree(fullex_devices + i);

    if (fullex_devices)
      kfree(fullex_devices);

    return ret_val;
}

void cleanup_module()
{
  int i;

  for (i = 0; i < ndevs; i++)
  {
    cdev_del(&fullex_devices[i].cdev); /* borra el registro del char device en el kernel*/
    fullex_kfree(fullex_devices + i); /* Liberamos la memoria de los datos privados */
  }

  kfree(fullex_devices); /* Libera la memoria de los dispositivos */

  unregister_chrdev_region(MKDEV(major, 0), ndevs);
}
