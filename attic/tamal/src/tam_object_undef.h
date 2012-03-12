#ifndef __TAM__OBJECT__UNDEF__
#define __TAM__OBJECT__UNDEF__

#include <tam_object.h>

/* create a new object of type undef : it will be returned by funtions
 * that should return nothing, ie: (eval (if #f 1)) */
TAMObject           *tam_object_undef_new         (TAMEnv      *env);

#endif
