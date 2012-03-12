#include <tamal.h>
#include <tam_error.h>
#include <tam_eval.h>
#include <tam_func.h>

typedef struct TAMEnvError TAMEnvError;

struct TAMEnv 
{
  /* code being executed.
   * This allows us to perform garbage collection anytime */
  TAMObject              *code; 
  
  /* garbage collector */
  TAMGarbage             *gc;

  /* symbol table */
  TAMSymbolTable         *symbol_tbl;

  /* shared nil object */
  TAMObject              *nil;

  /* shared special object unspecified */
  TAMObject              *undef;

  /* share quotation symbol "quote" */
  TAMObject              *quote;

  /* share the true object */
  TAMObject              *true;

  /* share false object */
  TAMObject              *false;

  /* a '.' */
  TAMObject              *dot;

  /* Lisp Core Module */
  TAMObject              *core_module;

  /* error information */
  TAMError *errors;
};

TAMEnv *tam_environment_new ( void )
{
  TAMEnv *env;
  int status;

  env = malloc(sizeof(TAMEnv));
  CHECK_MEM(env);

  env->code         = NULL;
  env->errors       = tam_error_new();
  env->gc           = tam_garbage_new();
  env->symbol_tbl   = tam_symbol_table_new(TAMAL_SYMBOLTBL_LEVEL_HASH_SIZE);

  ASSERT(env->errors);
  ASSERT(env->gc);
  ASSERT(env->symbol_tbl);
  
  /* env must be ready for objects to be added at this point */
  
  env->nil          = tam_object_nil_new(env);     /* null */
  env->undef        = tam_object_undef_new(env);   /* unspecified */
  env->quote        = tam_object_symbol_new("quote", env); /* ' */
  env->true         = tam_object_bool_new('t', env); /* #t */
  env->false        = tam_object_bool_new('f', env); /* #f */
  env->dot          = tam_object_symbol_new(".", env); /* dot */

  /* TODO:remove this! and add a function that creates modules ... */
  env->core_module  = tam_object_module_new("lispcore", env);

  ASSERT(env->dot);
  ASSERT(env->nil);
  ASSERT(env->undef);
  ASSERT(env->quote);
  ASSERT(env->true);
  ASSERT(env->false);
  ASSERT(env->core_module);


  /* TODO:remove this! and add a function that creates modules ... */
  status = tam_env_symbol_add (env, tam_object_symbol_new("coremodule", env),
                               env->core_module, 0);
  ASSERT(status);

  status = tam_env_symbol_add (env, tam_object_symbol_new("#t", env),
                                    env->true, 0)
        &&  tam_env_symbol_add (env, tam_object_symbol_new("#f", env),
                                    env->false, 0)
        && tam_env_symbol_add (env, tam_object_symbol_new("nil", env),
                                    env->nil, 0);
#if 0
#define sym(x) tam_object_symbol_new((x), env)
#define cons(x,y) tam_object_pair_new((x), (y), env)
#define proc(args,body) tam_object_proc_new((args),(body), env)

  if(status)
     tam_env_symbol_add (env, sym("oq"), /* another quotation */
         proc(cons(sym("x"), env->nil),
              cons(sym("x"), env->nil))
      ,0);
#endif

  if (!status)
    {
      fprintf(stderr, "%s : could not add initial symbols\n", program_name);
      exit(EXIT_FAILURE);
    }

  return env; 
}

void tam_env_free ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->gc);
  ASSERT(env->nil);
  ASSERT(env->undef);
  ASSERT(env->quote);
  ASSERT(env->true);
  ASSERT(env->false);
  ASSERT(env->dot);
  ASSERT(env->symbol_tbl);
  ASSERT(env->core_module);
  ASSERT(env->errors);
 
  /* TODO: shall we perform garbage collection here? */
  
  tam_error_free(env->errors);
  free(env->core_module);
  tam_object_free(env->quote);
  tam_object_free(env->undef);
  tam_object_free(env->nil);
  tam_object_free(env->true);
  tam_object_free(env->false);
  tam_object_free(env->dot);

  tam_symbol_table_free(env->symbol_tbl);
  tam_garbage_free(env->gc);

  free(env);
}

void tam_env_exec ( TAMEnv *env, TAMObject *code )
{
  TAMObject *result;
  ASSERT(env);
  ASSERT(code);

  /* note that we lose a referece to the old code in env->code.
   * Such references will be garbage-collected. */
  
  env->code = code;

  tam_env_sweep(env);

  if (TAMAL_ENV_PRINT_BEFORE_EVALUATING)
    {
      printf("[before eval:");
      tam_object_print(func_car(code, env));
      printf("]\n");
    }

  result = func_eval(func_car(code, env), env);
  
  if (!tam_env_error_test(env))
    {
      tam_object_print(result);
      printf("\n");
    }
  else
    {
      tam_env_error_print(env, stdout);
      tam_env_error_flush(env); 
    }

  env->code = NULL; /* let's free the code here */
  tam_env_sweep(env);

  if (TAMAL_GARBAGE_COLLECTION_INFO)
    printf("\n");
}

void tam_env_register_obj ( TAMEnv *env, TAMObject *obj )
{
  ASSERT(env);
  ASSERT(env->gc);
  ASSERT(obj);

  tam_garbage_register_object(env->gc, obj);
}

void tam_env_sweep ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->gc);
  ASSERT(env->nil);

  /* the used_set operations *must* be inside the operations clear and sweep */
  
  tam_garbage_clear(env->gc);

  if (env->code)
    tam_object_used_set(env->code);
  tam_object_used_set(env->nil); 
  tam_object_used_set(env->undef);
  tam_object_used_set(env->quote);
  tam_object_used_set(env->true);
  tam_object_used_set(env->false);
  tam_object_used_set(env->dot);
  tam_object_used_set(env->core_module);
  tam_symbol_table_used_set(env->symbol_tbl);

  tam_garbage_sweep(env->gc, stdout);
}

int tam_env_symbol_add ( TAMEnv *env, TAMObject *name, TAMObject *value, int overwrite )
{
  ASSERT(env);
  ASSERT(value);
  ASSERT(name);
  
  return tam_symbol_table_add(env->symbol_tbl, name, value, overwrite);
}

TAMObject *tam_env_symbol_lookup ( TAMEnv *env, TAMObject *name )
{
   TAMObject *value;

   ASSERT(env);
   ASSERT(env->symbol_tbl);
   ASSERT(name);
   
   if (tam_symbol_table_query(env->symbol_tbl, name, &value))
     return value;

   return NULL;
}

void tam_env_error_push ( TAMEnv *env, char *msg, TAMObject *obj )
{
  ASSERT(env);
  ASSERT(env->errors);
  ASSERT(msg);

  if (obj)
    {
      const char *obj_desc;
      int         obj_desc_len;
      const char *msg_desc = "associated object : %s";
      int         msg_desc_len = 22;
      char       *tmp;

      ASSERT(strlen(msg_desc) <= msg_desc_len); /* better safe than sorry */
      
      if (tam_object_str_can_get(obj))
        obj_desc = tam_object_str_get(obj);
      else
        obj_desc = tam_object_type_name_get(obj);
      
      ASSERT(obj_desc);

      obj_desc_len = strlen(obj_desc);
      
      tmp = malloc(obj_desc_len + msg_desc_len + 2);
      CHECK_MEM(tmp);
      snprintf(tmp, obj_desc_len + msg_desc_len + 2, msg_desc, obj_desc);
      tam_env_error_push(env, tmp, NULL); /* recursive call */
      free(tmp);
    }

  tam_error_push(env->errors, msg);
}

int tam_env_error_test ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->errors);

  return tam_error_isset(env->errors);
}

void tam_env_error_print ( TAMEnv *env, FILE   *f )
{
  ASSERT(env);
  ASSERT(env->errors);
  ASSERT(f);

  tam_error_print(env->errors, f);
}

void tam_env_error_flush ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->errors);

  tam_error_flush(env->errors);
}

TAMObject *tam_env_nil_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->nil);

  return env->nil;
}

TAMObject *tam_env_undef_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->undef);

  return env->undef;
}

TAMObject *tam_env_quote_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->undef);

  return env->quote;
}

TAMObject *tam_env_true_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->true);

  return env->true;
}

TAMObject *tam_env_false_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->false);

  return env->false;
}

TAMObject *tam_env_dot_get ( TAMEnv *env )
{
  ASSERT(env);
  ASSERT(env->dot);

  return env->dot;
}

void tam_env_extend (TAMEnv *env)
{ 
  ASSERT(env);
  ASSERT(env->symbol_tbl);
  
  tam_symbol_table_extend(env->symbol_tbl);
}    

void tam_env_shrink (TAMEnv *env)
{
  ASSERT(env);
  ASSERT(env->symbol_tbl);
  
  tam_symbol_table_shrink(env->symbol_tbl);
}
