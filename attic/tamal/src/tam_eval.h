#ifndef __TAM__EVAL__
#define __TAM__EVAL__

#include <tam_object.h>
#include <tam_environment.h>

/* Almighty eval */
TAMObject          *func_eval              (TAMObject *exp, TAMEnv *env);

#endif
