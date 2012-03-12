#include <tamal.h>
#include <tam_apply.h>
#include <tam_auxeval.h>
#include <tam_eval.h>

/* auxiliar functions */
static void empty_stack (TAMStack *s);

TAMObject *func_apply ( TAMObject *proc, TAMObject *call_arg_list, TAMEnv *env )
{
  TAMObject      *proc_arg_list, *return_obj=NULL;
  TAMObject      *error_obj = NULL;
  char           *error_str = NULL;
  TAMStack       *new_local_vars, *new_local_vars_evaluated, *rev_stack;
  int             has_opt_arg = 0;
  
  
  ASSERT(proc);
  ASSERT(call_arg_list);
  ASSERT(env);

  ASSERT(!tam_env_error_test(env));
  ASSERT(tam_object_istype_byname(proc, "proc"));

  new_local_vars = tam_stack_new(); 
  new_local_vars_evaluated = tam_stack_new();
  rev_stack = tam_stack_new();

  proc_arg_list = tam_object_proc_args_get(proc);
  ASSERT(proc_arg_list);

  while (!tam_object_istype_byname(proc_arg_list, "nil"))
    {
      TAMObject *var_name, *var_value_exp;

      var_name = tam_object_pair_car(proc_arg_list);
      ASSERT(tam_object_istype_byname(var_name, "symbol"));
      
      has_opt_arg = !strcmp(tam_object_str_get(var_name), ".");

      if (!has_opt_arg && tam_object_istype_byname(call_arg_list, "nil"))
        {
          error_str = "Missing expression in procedure application";
          goto apply_exit;
        }

      ASSERT(has_opt_arg || tam_object_istype_byname(call_arg_list, "pair"));

      if (has_opt_arg)
        {
          proc_arg_list = tam_object_pair_cdr(proc_arg_list);
          var_name = tam_object_pair_car(proc_arg_list);

          ASSERT(var_name);
          ASSERT(tam_object_istype_byname(var_name, "symbol"));
          ASSERT(tam_object_istype_byname(tam_object_pair_cdr(proc_arg_list),"nil"));

          var_value_exp = call_arg_list;
        }
      else
        {
          var_value_exp = tam_object_pair_car(call_arg_list);
          call_arg_list = tam_object_pair_cdr(call_arg_list);
        }
          

      tam_stack_push(new_local_vars, var_name, 0);
      tam_stack_push(new_local_vars, var_value_exp, 0);

      proc_arg_list = tam_object_pair_cdr(proc_arg_list);
    }

  if (!has_opt_arg && !tam_object_istype_byname(call_arg_list, "nil"))
    {
      error_str = "Extra expression in procedure application";
      goto apply_exit;
    }

  /* let's evaluated argument expressions */
  while (!tam_stack_isempty(new_local_vars))
    {
      TAMObject *arg_exp, *arg_name;
      tam_stack_pop (new_local_vars, (void*)&arg_exp, NULL);
      tam_stack_pop (new_local_vars, (void*)&arg_name, NULL);

      if (!has_opt_arg)
        {
          arg_exp = func_eval(arg_exp, env);

          if (tam_env_error_test(env))
            {
              error_str = "Error evaluating arguments";
              error_obj = arg_exp;

              goto apply_exit;
            }
        }
      else if (!tam_object_istype_byname(arg_exp, "nil"))
        {
          TAMObject *new_list, *tmp;

          /* let's evaluate each list item in the argument */

          ASSERT(rev_stack);

          while (!tam_object_istype_byname(arg_exp, "nil"))
            {
              ASSERT(tam_object_istype_byname(arg_exp, "pair"));
              
              tmp = func_eval(tam_object_pair_car(arg_exp), env);
              
              if (tam_env_error_test(env))
                {
                  error_str = "Error evaluating arguments";
                  error_obj = tam_object_pair_car(arg_exp);

                  goto apply_exit;
                }

              tam_stack_push(rev_stack, tmp, 0);

              arg_exp = tam_object_pair_cdr(arg_exp);
            }

          ASSERT(!tam_stack_isempty(rev_stack));

          new_list = tam_env_nil_get(env);

          while (!tam_stack_isempty(rev_stack))
            {
              tam_stack_pop(rev_stack, (void*)&tmp, NULL);
              new_list = tam_object_pair_new(tmp, new_list, env);
            }

          arg_exp = new_list;
        }

      has_opt_arg = 0; /* we're done with the optional arg */

      ASSERT(!tam_env_error_test(env));

      tam_stack_push(new_local_vars_evaluated, arg_name, 0);
      tam_stack_push(new_local_vars_evaluated, arg_exp, 0);
    }

  tam_env_extend(env); /* create a new frame in the symbol table */

  while (!tam_stack_isempty(new_local_vars_evaluated))
    {
      TAMObject *var_value, *var_name;
      tam_stack_pop (new_local_vars_evaluated, (void*)&var_value, NULL);
      tam_stack_pop (new_local_vars_evaluated, (void*)&var_name, NULL);

      tam_env_symbol_add(env, var_name, var_value, 0);
    }

  /* tam_object_proc_exec will call a callback or func_eval
   * depending on the kind of procedure : primitive or defined. */
  return_obj = tam_object_proc_exec(proc, env); 
  
  if (tam_env_error_test(env))
    {
      error_str = "Evaluating the body of the procedure";
      error_obj = tam_object_pair_car(tam_object_proc_args_get(proc));
      return_obj = NULL;
      goto apply_exit;
    }

  tam_env_shrink(env);
  
apply_exit:

  ASSERT(error_str || (return_obj && !error_obj && tam_stack_isempty(new_local_vars)));
  ASSERT(!error_str || !return_obj);
  
  ASSERT(new_local_vars_evaluated);
  ASSERT(new_local_vars);
  ASSERT(rev_stack);
  
  ASSERT(error_str || tam_stack_isempty(new_local_vars_evaluated));
  ASSERT(error_str || tam_stack_isempty(new_local_vars));
  ASSERT(error_str || tam_stack_isempty(rev_stack));

  if (error_str)
    {
      empty_stack(new_local_vars_evaluated);
      empty_stack(new_local_vars);
      empty_stack(rev_stack);
    }

  tam_stack_free(new_local_vars_evaluated);
  tam_stack_free(new_local_vars);
  tam_stack_free(rev_stack);
  
  if(error_str)
    {
      tam_env_error_push(env, error_str, error_obj);
      return tam_env_nil_get(env);
    }

  ASSERT(return_obj); /* redundant */

  return return_obj;
}

static void empty_stack (TAMStack *s)
{
  TAMObject *o;
  while (!tam_stack_isempty(s))
    tam_stack_pop(s, (void*)&o, NULL);
}

