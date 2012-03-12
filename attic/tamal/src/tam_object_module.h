#ifndef __TAM__OBJECT__MODULE__ 
#define __TAM__OBJECT__MODULE__

#include <tamal.h>
#include <tam_module.h>

/* create a new object */
TAMObject         *tam_object_module_new          ( char *module_name,
                                                    TAMEnv * env );
#endif
