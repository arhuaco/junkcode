#ifndef __TAM__FUNC__
#define __TAM__FUNC__

#include <tam_object.h>
#include <tam_environment.h>

#if 0
TAMObject          *func_eval              (TAMObject *exp, TAMEnv *env);
#endif

/* Functions for the nature of objects */
TAMObject          *func_isnull            (TAMObject *obj, TAMEnv *env);
TAMObject          *func_isnumber          (TAMObject *obj, TAMEnv *env);
TAMObject          *func_issymbol          (TAMObject *obj, TAMEnv *env);
TAMObject          *func_ispair            (TAMObject *obj, TAMEnv *env);

/* Functions that deal directly with lists */
TAMObject          *func_car               (TAMObject *obj, TAMEnv *env);
TAMObject          *func_cdr               (TAMObject *obj, TAMEnv *env);

#endif
