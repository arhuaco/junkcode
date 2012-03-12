#include <tamal.h>
#include <tam_auxeval.h>
#include <tam_func.h> /* <== don't use these functions anymore ... */
#include <tam_apply.h>

/* 
 * As a guide I am using the metacircular evaluator described in
 * "Structure and Interpretation of Computer Programs". Well, this
 * one is in C so it's not metacircular, anymore.
 * This is a great book, available on the Internet.
 */

static TAMObject * eval_lambda(TAMObject *exp, TAMEnv *env);

TAMObject *func_eval (TAMObject *exp, TAMEnv *env)
{
  ASSERT(exp);
  ASSERT(env);

  ASSERT(!tam_env_error_test(env));

  /* (self-evaluating? exp) */
  if (tam_object_istype_byname(exp, "bool")
         ||
      tam_object_istype_byname(exp, "number")
         ||
      tam_object_istype_byname(exp, "proc"))
    return exp;

  if (tam_object_istype_byname(exp, "nil"))
    {
      tam_env_error_push(env, "Unquoted ()", exp); 
      return tam_env_undef_get(env);
    }

  /* (variable? exp) */
  if (tam_object_istype_byname(exp, "symbol"))
    {
      TAMObject *value;

      value = tam_env_symbol_lookup(env, exp);
      if (value)
        return value;

      tam_env_error_push(env, "Unbound variable", exp); 
      return tam_env_undef_get(env);
    }

  /* quoted exp? */
  if (is_tagged_list(exp, "quote", env))
    {
      TAMObject *cdr_exp;
      unsigned int exp_len;

      exp_len = list_length(exp, env, 3);

      if(exp_len != 2)
        {
           char *msg;
           msg = exp_len > 2 ? "Extra expression in 'quote'" :
                               "Missing expression in 'quote'";

           tam_env_error_push(env, msg, NULL);
           return tam_env_undef_get(env);
        }

      cdr_exp = func_cdr(exp, env);
      ASSERT(tam_object_istype_byname(cdr_exp, "pair"));

      return func_car(cdr_exp, env);
    }
  
  /* lambda? */
  if (is_tagged_list(exp, "lambda", env))
    return  eval_lambda(tam_object_pair_cdr(exp), env);

  /* definition? */
  if (is_tagged_list(exp, "define", env))
    {
      unsigned int estimated_len;

      estimated_len = list_length(exp, env, 4);

      if (estimated_len < 3)
        {
           tam_env_error_push(env, "Missing expression in 'define'", NULL);
           return tam_env_undef_get(env);
        }
      else
        {
          TAMObject *name, *value, *lambda_formals = NULL;
          int is_symbol;

          name = func_car(func_cdr(exp, env), env);

          if (tam_object_istype_byname(name, "pair"))
            {
              lambda_formals = tam_object_pair_cdr(name);
              name = tam_object_pair_car(name);
            }
          else if (estimated_len > 3)
            {
              tam_env_error_push(env, "Extra expression in 'define'", NULL);
              return tam_env_undef_get(env);
            }

          is_symbol = tam_object_istype_byname(name, "symbol");

          if (!is_symbol || (is_symbol && !strcmp(tam_object_str_get(name), ".")))
            {
              tam_env_error_push(env, "Bad variable in 'define'", name);
              return tam_env_undef_get(env);
            }

          if (lambda_formals)
            value =
              eval_lambda(
              tam_object_pair_new(lambda_formals,
                                   func_cdr(func_cdr(exp,
                                                     env),
                                            env),
                                   env),
              env);
          
          else
            value =  func_eval(func_car(func_cdr(func_cdr(exp, env), env), env), env);
          if (tam_env_error_test(env))
            {
              if (tam_arg_errorfull)
                tam_env_error_push(env, "(extended info)  ... using 'define'",
                                   name);
              return tam_env_undef_get(env);
            }

          tam_env_symbol_add(env, name, value, 1); 

          return tam_env_undef_get(env); /* we won't return value! */
        }
    }

  /* if? */
  if (is_tagged_list(exp, "if", env))
    {
      unsigned int estimated_len;

      estimated_len = list_length(exp, env, 5);

      if (estimated_len != 3 && estimated_len != 4)
        {
           char *msg;
           msg = estimated_len > 4 ? "Extra expressions in 'if'"
                                   : "Missing expression in 'if'";

           tam_env_error_push(env, msg, NULL);
           return tam_env_undef_get(env);
        }
      else
        {
          TAMObject *cond, *yes, *no;
          TAMObject *before_cond, *before_yes, *before_no;

          cond = func_cdr(exp, env);
          yes  = func_cdr(cond, env);
          no   = estimated_len == 4 ? func_car(func_cdr(yes, env), env)
                                    : NULL;

          before_cond = func_car(cond, env);
          cond = func_eval(before_cond, env);

          if (tam_env_error_test(env))
            {
              if (tam_arg_errorfull)
                tam_env_error_push(env, "(extended info) Evaluating the condition of an 'if'", before_cond);
              return tam_env_undef_get(env);
            }

          if (!tam_object_istype_byname(cond, "bool")
              || tam_object_bool_istrue(cond))
            {
              before_yes = func_car(yes, env);

              yes = func_eval(before_yes, env);

            if (tam_env_error_test(env))
              {
                if (tam_arg_errorfull)
                  tam_env_error_push(env, "(extended info) Evaluating the 'yes' part of an 'if'", before_yes);
                return tam_env_undef_get(env);
              }

              return yes;
            }

          /* cond is false ... */
          
          if (!no)
            return tam_env_undef_get(env);

          no = func_eval((before_no = no), env);

          if (tam_env_error_test(env))
            {
              if (tam_arg_errorfull)
                tam_env_error_push(env, "(extended info) Evaluating the 'no' part of an 'if'", before_no);
              return tam_env_undef_get(env);
            }

          return no;
        }
    }

  /* application of a procedure? */
  if (tam_object_istype_byname(exp, "pair"))
    {
      TAMObject *op, *args, *result;

      op   = func_eval(func_car(exp, env), env);

      if (tam_env_error_test(env))
        {
          if (tam_arg_errorfull)
            tam_env_error_push(env, "(extended info) Evaluating the operator to 'apply'", NULL);
          return tam_env_undef_get(env);
        }

      if (!tam_object_istype_byname(op, "proc"))
        {
          tam_env_error_push(env, "Wrong type to 'apply'. I expected a procedure.", op);
          return tam_env_undef_get(env);
        }

      args = func_cdr(exp, env);

      result = func_apply(op, args, env); 

      if (tam_env_error_test(env))
        {
          if (tam_arg_errorfull)
            tam_env_error_push(env, "(extended info) Aplying operator", op);
        
          return tam_env_undef_get(env);
        }

      return result;
    }

  printf ("Error in eval!\n");
  ASSERT (0);
  exit(1);
}

/*
 * optimization : lambda will come without the leading symbol "lambda"
 */

TAMObject * eval_lambda(TAMObject *exp, TAMEnv *env)
{
  unsigned int estimated_len;
  TAMObject *formals;
  TAMObject *body;

  /* TODO: we could optimize here adding another argument to eval_lambda
   * for the list_lenght */
  
  estimated_len = list_length(exp, env, 2); 

  if (estimated_len < 2)
    {
       tam_env_error_push(env, "Missing expression in 'lambda'", NULL);
       return tam_env_undef_get(env);
    }

  formals = tam_object_pair_car(exp);
  body    = tam_object_pair_cdr(exp);

  tam_object_proc_formals_validate(formals, env, 1 /* pushes error if any */);
  
  if (tam_env_error_test(env))
     return tam_env_nil_get(env);
    
  tam_object_proc_body_validate(body, env, 1 /* pushes error if any */);

  if(tam_env_error_test(env))
    return tam_env_nil_get(env);

  if (!tam_object_istype_byname(formals, "pair") && !tam_object_istype_byname(formals, "nil"))
    formals = tam_object_pair_new(formals, tam_env_nil_get(env), env);

  return tam_object_proc_new(formals, body, env);
}
