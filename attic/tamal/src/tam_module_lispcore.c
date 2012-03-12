#include <tamal.h>
#include <tam_module.h>
#include <tam_eval.h>

static void     *(module_lispcore_init)           ( TAMEnv *env );
static void      (module_lispcore_free)           ( void *data  );
static void      (module_lispcore_setused)         ( void *data );

typedef struct LispCoreData LispCoreData;

struct LispCoreData
{
  TAMObject *arg_list_cache_1; /* (arg1) */
  TAMObject *arg_list_cache_2; /* (arg1 arg2) */
  TAMObject *arg_list_cache_3; /* ( . rest) */
  TAMObject *arg1;
  TAMObject *arg2;
  TAMObject *rest;
};

TAMModuleType TAMModuleLispCore = 
{
      "lispcore",
       NULL,
       module_lispcore_init,
       module_lispcore_free,
       module_lispcore_setused
};

/* initialize args */
#define NEWARGS(x_arg) \
        x_arg = tam_primitive_init_args(env)

/* remember to add args in inverse order */
#define ADDARG(x_arg, x_new_arg) \
        x_arg = tam_primitive_add_to_args(x_arg, x_new_arg, env)

/* return type of all the callbacks */
#define OBJ TAMObject
/* arguments of all the callbacks */
#define ARGS TAMObject *arguments, void *data, TAMEnv *env

/* auxiliar functions */

/* is the object an integer? */
INLINE static int  is_int    (TAMObject *obj);

/* is the object of type bool and value #f? */
INLINE static int is_false   (TAMObject *obj);

/* primitive procedures that this module will create */

/* lists */
OBJ *core_eval        (ARGS);        /* (eval arg1) */
OBJ *core_car         (ARGS);        /* (car arg1) */
OBJ *core_cdr         (ARGS);        /* (cdr arg1) */
OBJ *core_cons        (ARGS);        /* (cons arg1 arg2) */
OBJ *core_is_null     (ARGS);        /* (null? arg1) */
OBJ *core_is_pair     (ARGS);        /* (pair? arg1) */

/* basic math operations */
OBJ *core_add         (ARGS);        /* (+ . rest) */ 
OBJ *core_sub         (ARGS);        /* (- . rest) */
OBJ *core_mul         (ARGS);        /* (* . rest) */
OBJ *core_div         (ARGS);        /* (/ . rest) */
OBJ *core_mod         (ARGS);        /* (remainder arg1 arg2) */

/* basic logic operators */

OBJ *core_and         (ARGS);        /* (and . rest) */
OBJ *core_or          (ARGS);        /* (or . rest) */
OBJ *core_not         (ARGS);        /* (not arg1) */

/* basic comparisions */

OBJ *core_gt          (ARGS);        /* (> . rest) */
OBJ *core_gte         (ARGS);        /* (> . rest) */
OBJ *core_lt          (ARGS);        /* (> . rest) */
OBJ *core_lte         (ARGS);        /* (> . rest) */
OBJ *core_number_eq   (ARGS);        /* (= rest) */

static void *module_lispcore_init ( TAMEnv *env )
{
  LispCoreData *d;

  ASSERT(env);

  d = malloc(sizeof(LispCoreData));
  CHECK_MEM(d);

  /* the space here is important! this is a symbol that the user will not
   * be able to access ... */
  d->arg1 = tam_object_symbol_new(" arg1", env);
  d->arg2 = tam_object_symbol_new(" arg2", env);
  d->rest = tam_object_symbol_new(" rest", env);
  
  NEWARGS(d->arg_list_cache_1); /* (arg1) */
  ADDARG(d->arg_list_cache_1, d->arg1);
  ASSERT(d->arg_list_cache_1);
  ASSERT(tam_object_proc_formals_validate(d->arg_list_cache_1, env, 0));

  NEWARGS(d->arg_list_cache_2); /* (arg1 arg2) */
  ADDARG(d->arg_list_cache_2, d->arg2);
  ADDARG(d->arg_list_cache_2, d->arg1);
  ASSERT(d->arg_list_cache_2);
  ASSERT(tam_object_proc_formals_validate(d->arg_list_cache_1, env, 0));

  NEWARGS(d->arg_list_cache_3);
  ADDARG(d->arg_list_cache_3, d->rest);
  ADDARG(d->arg_list_cache_3, tam_env_dot_get(env)); 
  ASSERT(d->arg_list_cache_3);
  ASSERT(tam_object_proc_formals_validate(d->arg_list_cache_3, env, 0));

  tam_primitive_register("eval", core_eval, d, d->arg_list_cache_1, env);
  tam_primitive_register("car", core_car, d, d->arg_list_cache_1, env);
  tam_primitive_register("cdr", core_cdr, d, d->arg_list_cache_1, env);
  tam_primitive_register("null?", core_is_null, d, d->arg_list_cache_1, env);
  tam_primitive_register("pair?", core_is_pair, d, d->arg_list_cache_1, env);
  tam_primitive_register("cons", core_cons, d, d->arg_list_cache_2, env);

  tam_primitive_register("remainder", core_mod, d, d->arg_list_cache_2, env);
  tam_primitive_register("+", core_add, d, d->arg_list_cache_3, env);
  tam_primitive_register("-", core_sub, d, d->arg_list_cache_3, env);
  tam_primitive_register("*", core_mul, d, d->arg_list_cache_3, env);
  tam_primitive_register("/", core_div, d, d->arg_list_cache_3, env);

  tam_primitive_register("and", core_and, d, d->arg_list_cache_3, env);
  tam_primitive_register("or", core_or, d, d->arg_list_cache_3, env);
  tam_primitive_register("not", core_not, d, d->arg_list_cache_1, env);

  tam_primitive_register(">", core_gt, d, d->arg_list_cache_3, env);
  tam_primitive_register(">=", core_gte, d, d->arg_list_cache_3, env);
  tam_primitive_register("<", core_lt, d, d->arg_list_cache_3, env);
  tam_primitive_register("<=", core_lte, d, d->arg_list_cache_3, env);
  tam_primitive_register("=", core_number_eq, d, d->arg_list_cache_3, env);

  return d;
}

static void module_lispcore_free ( void *data )
{
  LispCoreData *d = data;

  ASSERT(d);

  /* d->arg_cacheXX will be garbage collected */

  free(d);
}

static void module_lispcore_setused ( void *data )
{
  LispCoreData *d = data;
  ASSERT(d);

  tam_object_used_set(d->arg_list_cache_1);
  tam_object_used_set(d->arg_list_cache_2);
}

int is_int (TAMObject *obj)
{
  int x;
  double d;

  ASSERT(obj);
  
  if(!tam_object_istype_byname(obj, "number"))
    return 0;
  
  d = tam_object_number_value_get(obj);
  x = (int) d;

  return (double)x == d;
}

int is_false (TAMObject *obj)
{
  ASSERT(obj);
  
  return tam_object_istype_byname(obj, "bool")
         &&
         !tam_object_bool_istrue(obj);
}


TAMObject *core_eval (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  return func_eval(arg1, env);
}

TAMObject *core_car (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  if (!tam_object_istype_byname(arg1, "pair"))
    {
      tam_env_error_push(env, "Primitive 'car' : Bad argument", arg1);
      return tam_env_nil_get(env);
    }
  
  return tam_object_pair_car(arg1);
}

TAMObject *core_cdr (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  if (!tam_object_istype_byname(arg1, "pair"))
    {
      tam_env_error_push(env, "Primitive 'cdr' : Bad argument", arg1);
      return tam_env_nil_get(env);
    }
  
  return tam_object_pair_cdr(arg1);
}

TAMObject *core_is_null (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  return tam_object_istype_byname(arg1, "nil")
           ? tam_env_true_get(env)
           : tam_env_false_get(env);
}

TAMObject *core_is_pair (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  return tam_object_istype_byname(arg1, "pair")
           ? tam_env_true_get(env)
           : tam_env_false_get(env);
}

TAMObject *core_cons (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1, *arg2;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_2);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);
  arg2 = tam_env_symbol_lookup(env, d->arg2);

  ASSERT(arg1);
  ASSERT(arg2);

  return tam_object_pair_new(arg1, arg2, env);
}

TAMObject *core_add (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  unsigned int i = 1;
  char error[48];
  double result = 0.0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      ASSERT(tam_object_istype_byname(arg, "pair"));

      x = tam_object_pair_car(arg);
      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          snprintf(error, 47, "Arg %u is not a number in +", i);
          tam_env_error_push(env, error, x);
          return tam_env_nil_get(env);
        }

      result += tam_object_number_value_get(x);
      arg = tam_object_pair_cdr(arg);
      i++;
    }

  return tam_object_number_dbl_new(result, env);
}

TAMObject *core_sub (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  unsigned int i = 1;
  char error[48];
  double result;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      ASSERT(tam_object_istype_byname(arg, "pair"));

      x = tam_object_pair_car(arg);
      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          snprintf(error, 47, "Arg %u is not a number in -", i);
          tam_env_error_push(env, error, x);
          return tam_env_nil_get(env);
        }

      if (i != 1)
        result -= tam_object_number_value_get(x);
      else
        result = tam_object_number_value_get(x);

      arg = tam_object_pair_cdr(arg);
      i++;
    }

    if (i == 1)
      {
        snprintf(error, 47, "At least 1 argument required in -");
        tam_env_error_push(env, error, NULL);
        return tam_env_nil_get(env);
      }
    else if (i == 2)
      return tam_object_number_dbl_new(-result, env);

  return tam_object_number_dbl_new(result, env);
}

TAMObject *core_mul (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  unsigned int i = 1;
  char error[48];
  double result = 1.0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      ASSERT(tam_object_istype_byname(arg, "pair"));

      x = tam_object_pair_car(arg);
      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          snprintf(error, 47, "Arg %u is not a number in *", i);
          tam_env_error_push(env, error, x);
          return tam_env_nil_get(env);
        }

      result *= tam_object_number_value_get(x);
      arg = tam_object_pair_cdr(arg);
      i++;
    }

  return tam_object_number_dbl_new(result, env);
}

TAMObject *core_div (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  unsigned int i = 1;
  char error[48];
  double result;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      ASSERT(tam_object_istype_byname(arg, "pair"));

      x = tam_object_pair_car(arg);
      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          snprintf(error, 47, "Arg %u is not a number in /", i);
          tam_env_error_push(env, error, x);
          return tam_env_nil_get(env);
        }

      if (i != 1)
        result /= tam_object_number_value_get(x);
      else
        result = tam_object_number_value_get(x);

      arg = tam_object_pair_cdr(arg);
      i++;
    }

    if (i == 1)
      {
        snprintf(error, 47, "At least 1 argument required in /");
        tam_env_error_push(env, error, NULL);
        return tam_env_nil_get(env);
      }
    else if (i == 2)
      return tam_object_number_dbl_new(1.0 / result, env);

  return tam_object_number_dbl_new(result, env);
}

TAMObject *core_mod (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject    *arg1, *arg2;
  char         *error_msg = NULL;
  TAMObject    *error_obj = NULL;
  int           a, b;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_2);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);
  arg2 = tam_env_symbol_lookup(env, d->arg2);

  ASSERT(arg1);
  ASSERT(arg2);

  if (!is_int(arg1))
    {
      error_msg = "Invalid argument (arg1) to 'remainder'";
      error_obj = arg1;
    }
  else if (!is_int(arg2))
    {
      error_msg = "Invalid argument (arg2) to 'remainder'";
      error_obj = arg2;
    }
  else if ((b = (int)tam_object_number_value_get(arg2)) == 0)
    {
      error_msg = "(arg2) cannot be zero in 'remainder'";
      error_obj = arg2;
    }
  else
    {
      a = (int)tam_object_number_value_get(arg1); 
      return tam_object_number_dbl_new(a % b, env);
    }

  ASSERT(error_msg);

  tam_env_error_push(env, error_msg, error_obj);
  return tam_env_nil_get(env);
}

TAMObject *core_and (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg, *last = NULL;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      last = tam_object_pair_car(arg);

      ASSERT(last);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (is_false(last))
        return last;

      arg = tam_object_pair_cdr(arg);
    }

  if (last)
    return last;

  return tam_env_true_get(env);
}

TAMObject *core_or (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!is_false(x))
        return x;

      arg = tam_object_pair_cdr(arg);
    }

  return tam_env_false_get(env);
}

TAMObject *core_not (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg1;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_1);
  
  arg1 = tam_env_symbol_lookup(env, d->arg1);

  ASSERT(arg1);

  return tam_object_bool_new(is_false(arg1) ? 't' : 'f', env);
}

TAMObject *core_gt (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  double first;
  unsigned int i = 0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      i++;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          char *str;

          str = malloc(36);
          snprintf(str, 35, "Bad argument (Arg%d) to >", i);
          str[35] = 0;
          tam_env_error_push(env, str, x);
          free(str);

          return tam_env_nil_get(env);
        }

      if (i == 1)
        first = tam_object_number_value_get(x);
      else if (first <= tam_object_number_value_get(x))
            return tam_env_false_get(env);

      arg = tam_object_pair_cdr(arg);
    }

  if (i < 2)
    {
      tam_env_error_push(env, "less than two arguments to >", NULL);
      return tam_env_nil_get(env);
    }

  return tam_env_true_get(env);
}

TAMObject *core_gte (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  double first;
  unsigned int i = 0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      i++;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          char *str;

          str = malloc(36);
          snprintf(str, 35, "Bad argument (Arg%d) to >=", i);
          str[35] = 0;
          tam_env_error_push(env, str, x);
          free(str);

          return tam_env_nil_get(env);
        }

      if (i == 1)
        first = tam_object_number_value_get(x);
      else if (first < tam_object_number_value_get(x))
            return tam_env_false_get(env);

      arg = tam_object_pair_cdr(arg);
    }

  if (i < 2)
    {
      tam_env_error_push(env, "less than two arguments to >=", NULL);
      return tam_env_nil_get(env);
    }

  return tam_env_true_get(env);
}

TAMObject *core_lt (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  double first;
  unsigned int i = 0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      i++;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          char *str;

          str = malloc(36);
          snprintf(str, 35, "Bad argument (Arg%d) to <", i);
          str[35] = 0;
          tam_env_error_push(env, str, x);
          free(str);

          return tam_env_nil_get(env);
        }

      if (i == 1)
        first = tam_object_number_value_get(x);
      else if (first >= tam_object_number_value_get(x))
            return tam_env_false_get(env);

      arg = tam_object_pair_cdr(arg);
    }

  if (i < 2)
    {
      tam_env_error_push(env, "less than two arguments to <", NULL);
      return tam_env_nil_get(env);
    }

  return tam_env_true_get(env);
}

TAMObject *core_lte (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  double first;
  unsigned int i = 0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      i++;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          char *str;

          str = malloc(36);
          snprintf(str, 35, "Bad argument (Arg%d) to <=", i);
          str[35] = 0;
          tam_env_error_push(env, str, x);
          free(str);

          return tam_env_nil_get(env);
        }

      if (i == 1)
        first = tam_object_number_value_get(x);
      else if (first > tam_object_number_value_get(x))
            return tam_env_false_get(env);

      arg = tam_object_pair_cdr(arg);
    }

  if (i < 2)
    {
      tam_env_error_push(env, "less than two arguments to <=", NULL);
      return tam_env_nil_get(env);
    }

  return tam_env_true_get(env);
}

TAMObject *core_number_eq (TAMObject *arguments, void *data, TAMEnv *env)
{
  LispCoreData *d = data; 
  TAMObject *arg;
  double first;
  unsigned int i = 0;

  ASSERT(arguments);
  ASSERT(data);
  ASSERT(env);

  ASSERT(arguments == d->arg_list_cache_3);
  
  arg = tam_env_symbol_lookup(env, d->rest);

  ASSERT(arg);

  while (!tam_object_istype_byname(arg, "nil"))
    {
      TAMObject *x;

      i++;

      x = tam_object_pair_car(arg);

      ASSERT(x);

      if (tam_env_error_test(env))
        return tam_env_nil_get(env);

      if (!tam_object_istype_byname(x, "number"))
        {
          char *str;

          str = malloc(36);
          snprintf(str, 35, "Bad argument (Arg%d) to =", i);
          str[35] = 0;
          tam_env_error_push(env, str, x);
          free(str);

          return tam_env_nil_get(env);
        }

      if (i == 1)
        first = tam_object_number_value_get(x);
      else if (first != tam_object_number_value_get(x))
            return tam_env_false_get(env);

      arg = tam_object_pair_cdr(arg);
    }

  if (i < 2)
    {
      tam_env_error_push(env, "less than two arguments to =", NULL);
      return tam_env_nil_get(env);
    }

  return tam_env_true_get(env);
}
