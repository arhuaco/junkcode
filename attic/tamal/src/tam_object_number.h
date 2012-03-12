#ifndef __TAM__OBJECT__NUMBER__
#define __TAM__OBJECT__NUMBER__

#include <tam_object.h>

/* create a new object of type number in an environment, from a string */
TAMObject *tam_object_number_new         ( char *str, TAMEnv *env );

/* create a new object of type number in an environment, from the value */
TAMObject *tam_object_number_dbl_new     ( double number, TAMEnv *env );

double     tam_object_number_value_get   ( TAMObject *obj );

#endif
