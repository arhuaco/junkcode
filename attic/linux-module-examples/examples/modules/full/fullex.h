#ifndef __FULLEX_H__
#define __FULLEX_H__

#include <linux/ioctl.h> /* Se necesita para  _IOW, _IOR, _IO */

#define FULLEX_IOC_MAGIC  'k'

#define FULLEX_RESET      _IO  (FULLEX_IOC_MAGIC, 0)
#define FULLEX_SETSIZE    _IOW (FULLEX_IOC_MAGIC, 1, int)
#define FULLEX_GETSIZE    _IOR (FULLEX_IOC_MAGIC, 2, int)

#define FULLEX_IOC_MAXNR   2

#endif 
