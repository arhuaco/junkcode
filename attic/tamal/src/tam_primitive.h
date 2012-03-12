#ifndef __TAM__PRIMITIVE__
#define __TAM__PRIMITIVE__

#include <tam_object.h>

/* Type of primitive procedures */
typedef TAMObject        *(TAMPrimitive)             ( TAMObject *arg,
                                                       void      *cb_data,
                                                       TAMEnv    *env );

/* create a new list of arguments */
TAMObject                *tam_primitive_init_args    ( TAMEnv    *env );

/* add a symbol to the list of arguments*/
TAMObject                *tam_primitive_add_to_args  ( TAMObject *args,
                                                       TAMObject *newarg,
                                                       TAMEnv    *env );

/* register a primitive procedure in the environment */
int                       tam_primitive_register     ( char      *name,
                                                       TAMPrimitive *callback,
                                                       void      *callback_data,
                                                       TAMObject *args,
                                                       TAMEnv    *env );
#endif

