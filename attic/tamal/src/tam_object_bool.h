#ifndef __TAM__OBJECT__BOOL__
#define __TAM__OBJECT__BOOL__

#include <tam_object.h>

/* create a new object of type symbol. it can be 't' o 'f' */
TAMObject           *tam_object_bool_new           ( char        value,
                                                     TAMEnv      *env);

/* returns non-zero if obj is true '#t' */
int                  tam_object_bool_istrue        ( TAMObject *obj );
#endif
