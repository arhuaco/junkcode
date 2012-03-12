#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/sort.h>

int cmp(const int *a, const int *b)
{
  if (*a > *b)
    return 1;
  if (*a < *b)
    return -1;
  return 0;
}

#define PROC_NAME "sort"

MODULE_LICENSE("Dual BSD/GPL");

int size = 8;

static int proc_count_show(struct seq_file *m, void *dummy)
{
  int i;
  int j;
  int *v;
  long t1;
  long t2;
  long ms;

  v = kmalloc(size * sizeof(int), GFP_KERNEL);

  for (i = 0; i < size; ++i)
    v[i] = size - i;

  t1 = jiffies;

  for (i = 0; i < size - 1; ++i)
    for (j = i + 1; j < size; ++j)
      if (v[i] > v[j])
      {
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
     }

  /* Jiffies can eventually wrap! you should not measure time like this */

  t2 = jiffies;
  ms = ((t2 - t1) * 1000) / HZ;

  seq_printf(m, "size:%d time:%lums jiffies:%lu\n", size, ms, (t2 - t1));

  for (i = 0; i < size; ++i)
    v[i] = size - i;

  t1 = jiffies;

  sort(v, size, sizeof(int), cmp, NULL);

  t2 = jiffies;
  ms = ((t2 - t1) * 1000) / HZ;

  seq_printf(m, "size:%d time:%lums jiffies:%lu\n", size, ms, (t2 - t1));

  kfree(v);

  size *= 2;

  return 0;
}

static int proc_count_open(struct inode *inode, struct file *file)
{
  return single_open(file, proc_count_show, NULL);
}

/*
 * Aca se definen las funciones que manejan las operaciones comunes.
 * .open es la funcion de abrir el archivo del /proc
 * .read es la funcion para leer
 * .llseek es la que se llama cuando se le hace fseek (miren que se usa una por
 *         defecto que ya está, si uno quiere puede usar una propia).
 * .release para cerrar el archivo.
 *
 * Las funciones que no se definen en aca estan en otro lado. Por ejemplo,
 * la funcion single_release esta en fs/seq_file.c
 *
 */

static struct file_operations proc_count_operations = {
  .open     = proc_count_open,
  .read     = seq_read,
  .llseek   = seq_lseek,
  .release  = single_release,
};

static int count_init(void)
{
 struct proc_dir_entry *e;

 e = create_proc_entry(PROC_NAME, 0, NULL);

 /* e es un puntero a la estructura que representa nuestro archivo
  * virtual en el /proc */

 if (e)
 {
  /* aca asignamos las funciones que van a manejar los eventos en
   * la entrada del /proc */
   e->proc_fops = &proc_count_operations;
 }
 else
 {
   printk(KERN_ERR "/proc/" PROC_NAME " creation failed \n");

   /* FIXME: tenemos que retornar aca un uno porque no se pudo crear el
    * "archivo"? Que pasa si retorno un uno en esta funciOn y un cero? */
 }

  return 0;
}

static void count_exit(void)
{
  remove_proc_entry(PROC_NAME, NULL);
}

module_init(count_init);
module_exit(count_exit);
