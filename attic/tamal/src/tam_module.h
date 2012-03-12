#ifndef __TAM__MODULE__
#define __TAM__MODULE__

typedef struct TAMModuleType TAMModuleType;

#include <tamal.h>

/*
 * This file defines the basic shape of a module.
 * Modules will be defined only as objects in the evaluation
 * environment. See tam_object_module[ch] for details.
 * 
 * The interface function init, free and setused are mandatory.
 */

/* Initalize the module. Must return custom data relative to this environment */
typedef void    *(module_init)             ( TAMEnv *env );

/* Free the module specific data. Data is relative to the environment */
typedef void      (module_free)            ( void *data );

/* Mark the objects used within the environment as used to avoid garbage collection */
typedef void      (module_setused)         ( void *data );

struct TAMModuleType
{
  /* name of the module */
  char                  *name;
  
  /* static data shared by the module in all the environments */
  void                  *local_module_data;

  /* function where you will initialize your module */
  module_init           *init;
  
  /* function where you will free your modules */
  module_free           *free;

  /* function that tells the environment not to release your objects */
  module_setused        *setused;
};


/* get the type of the module */
TAMModuleType      *tam_module_type_by_name_get    (char *name);

#endif
