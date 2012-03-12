#include <tamal.h>
#include <tam_primitive.h>
/*
 * This allows us to define primitive functions.
 * This will also help a lot when defining new primitives.
 */
#if 0
#define NEWARG tam_primitive_init_args
#define ADDARG tam_primitive_add_to_args

TAMObject * primitive_car ( TAMObject *arg, TAMEnv *env )
{
}
#endif

TAMObject * tam_primitive_init_args ( TAMEnv *env )
{
  ASSERT(env);
  return tam_env_nil_get(env);
}

TAMObject * tam_primitive_add_to_args (TAMObject *args, TAMObject *newarg, TAMEnv *env)
{
  ASSERT(args);
  ASSERT(newarg);
  ASSERT(env);

  ASSERT(tam_object_istype_byname(args, "nil") || tam_object_istype_byname(args, "pair"));

  return tam_object_pair_new(newarg, args, env);
}

int tam_primitive_register (char *name, TAMPrimitive *callback, void *callback_data, TAMObject *args, TAMEnv *env)
{
  int result;

  ASSERT(name);
  ASSERT(callback);
  ASSERT(args);
  ASSERT(env);
  
  result = tam_env_symbol_add(env,
                              tam_object_symbol_new(name, env),
                              tam_object_proc_callback_new(name,
                                                           args,
                                                           callback,
                                                           callback_data,
                                                           env),
                              0);

  ASSERT(result); 

  return result;
}
