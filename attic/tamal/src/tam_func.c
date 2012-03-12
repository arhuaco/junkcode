#include <tamal.h>

#include <tam_func.h>

/*
 * Functions that can be used from C.
 * I should not use these functions, and they will be removed soon.
 * The module lisp-core does everything now.
 */ 


/*
 * Macros that might be uses many times in this file
 * TODO: this is no longer true
 */

#define RETURN_UNDEF_IF_ERROR(ENV)                         \
  if (tam_env_error_test(ENV))                           \
    return tam_env_undef_get(ENV)

/*
 * TODO: remove this
 */
  
#define  DEFINE_ERROR_AND_RETURN(ENV,MSG,OBJ)            \
  tam_env_error_push(ENV, MSG, OBJ);                     \
  return tam_env_undef_get(ENV)
  
/*
 * Functions that help us test the nature of objects
 */

TAMObject *func_isnull (TAMObject *obj, TAMEnv *env)
{
  char value;

  ASSERT(env);
  ASSERT(obj);

  RETURN_UNDEF_IF_ERROR(env);

  value = tam_object_istype_byname(obj, "nil") ? 't' : 'f';

  return tam_object_bool_new(value, env);
}

TAMObject *func_isnumber (TAMObject *obj, TAMEnv *env)
{
  char value = 'f';

  ASSERT(obj);
  ASSERT(env);

  RETURN_UNDEF_IF_ERROR(env);

  if (tam_object_istype_byname(obj, "number"))
    value = 't';

  return tam_object_bool_new(value, env);
}

TAMObject *func_issymbol (TAMObject *obj, TAMEnv *env)
{
  char value = 'f';

  ASSERT(obj);
  ASSERT(env);

  RETURN_UNDEF_IF_ERROR(env);

  if (tam_object_istype_byname(obj, "symbol"))
    value = 't';

  return tam_object_bool_new(value, env);
}

TAMObject *func_ispair (TAMObject *obj, TAMEnv *env)
{
  char value = 'f';

  ASSERT(obj);
  ASSERT(env);

  RETURN_UNDEF_IF_ERROR(env);

  if (tam_object_istype_byname(obj, "pair"))
    value = 't';

  return tam_object_bool_new(value, env);
}

/*
 * Functions that 
 */

TAMObject *func_car (TAMObject *obj, TAMEnv *env)
{
  ASSERT(obj);
  ASSERT(env);

  RETURN_UNDEF_IF_ERROR(env);
  
  if(!tam_object_istype_byname(obj, "pair"))
    {
      DEFINE_ERROR_AND_RETURN(env,"car : invalid argument", obj);
    }

  return tam_object_pair_car(obj);
}

TAMObject *func_cdr (TAMObject *obj, TAMEnv *env)
{
  ASSERT(obj);
  ASSERT(env);

  RETURN_UNDEF_IF_ERROR(env);
  
  ASSERT(tam_object_istype_byname(obj, "pair")); /* TODO: handle error */

  if(!tam_object_istype_byname(obj, "pair"))
    {
      DEFINE_ERROR_AND_RETURN(env,"cdr : invalid argument", obj);
    }

  return tam_object_pair_cdr(obj);
}

