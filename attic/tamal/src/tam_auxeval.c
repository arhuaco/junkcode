#include <tamal.h>
#include <tam_auxeval.h>
#include <tam_func.h> /* don't use there functions anymore */

int is_tagged_list (TAMObject *exp, char *tag, TAMEnv *env)
{
  int r = 0;

  ASSERT(exp);
  ASSERT(env);
  ASSERT(tag);
  ASSERT(!tam_env_error_test(env));

  if (tam_object_bool_istrue(func_ispair(exp, env)))
    {
      TAMObject *car_exp;
      
      car_exp = func_car(exp, env);

      if (tam_object_bool_istrue(func_issymbol(car_exp, env)))
        {
          return !strcmp(tam_object_str_get(car_exp), tag);
        }
    }

  return r;
}

unsigned int list_length (TAMObject *obj, TAMEnv *env, unsigned int max)
{
  unsigned int l = 0;

  ASSERT(env);
  ASSERT(!tam_env_error_test(env));
  
  while (1)
    {
      ASSERT(obj);

      if ((max && max == l) || tam_object_istype_byname(obj, "nil"))
        return l;

      ASSERT(tam_object_istype_byname(obj, "pair"));
  
      obj = func_cdr(obj, env);
      ++l;
    }
}

