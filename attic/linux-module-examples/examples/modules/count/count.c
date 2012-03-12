#include <linux/init.h>
#include <linux/module.h>

#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#define PROC_NAME "count"

MODULE_LICENSE("Dual BSD/GPL");

int count = 0;

static int proc_count_show(struct seq_file *m, void *v)
{
  seq_printf(m, "%d\n", count++);

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
