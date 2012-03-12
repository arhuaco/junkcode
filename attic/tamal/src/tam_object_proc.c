#include <tamal.h>
#include <tam_primitive.h>
#include <tam_eval.h>

typedef struct TAMObjectProc TAMObjectProc;

struct TAMObjectProc
{
  char          *name;
  TAMObject     *args;
  TAMObject     *body;
  TAMPrimitive  *callback;
  void          *callback_data;
};

static void            *tam_object_proc_dup          ( void * );
static void             tam_object_proc_free         ( void * );
static void             tam_object_proc_print        ( void * );
static void             tam_object_proc_setused      ( void *, int use );
static  const char     *tam_object_proc_str          ( void * );

static TAMObjectType TAMObjectProcType =
{
  "proc",
  NULL,
  tam_object_proc_dup,
  tam_object_proc_free,
  tam_object_proc_print,
  tam_object_proc_setused,
  tam_object_proc_str
};


TAMObject *tam_object_proc_new (TAMObject *args,  TAMObject *body, TAMEnv * env)
{
  TAMObjectProc *new;

  ASSERT(env);
  ASSERT(args);
  ASSERT(body);
 
  new = malloc(sizeof(TAMObjectProc));
  CHECK_MEM(new);

  new->name = strdup("");
  CHECK_MEM(new->name);

  new->args     = tam_object_duplicate(args);
  new->body     = tam_object_duplicate(body);
  new->callback = NULL;
  new->callback_data = NULL;

  return tam_object_new(new, &TAMObjectProcType, env);
}

TAMObject *tam_object_proc_callback_new ( char *name, TAMObject *args, TAMPrimitive *func,
                                          void *func_data, TAMEnv *env )
{
  TAMObjectProc *new;

  ASSERT(args);
  ASSERT(func);
  ASSERT(env);
 
  new = malloc(sizeof(TAMObjectProc));
  CHECK_MEM(new);

  new->name = strdup(name);
  CHECK_MEM(new->name);


  new->args     = tam_object_duplicate(args);
  new->body     = NULL;
  new->callback = func;
  new->callback_data = func_data;

  return tam_object_new(new, &TAMObjectProcType, env);
}

void *tam_object_proc_dup ( void *data )
{
  return data; 
}

void tam_object_proc_free ( void *data )
{
  TAMObjectProc *p = data;
  
  ASSERT(p);
  ASSERT(p->name);
  
  free(p->name);
  free(p);
}

void tam_object_proc_print ( void *data )
{
  TAMObjectProc *p = data;
  ASSERT(data);
  ASSERT(p->args);
  ASSERT(p->body || p->callback);
  ASSERT(p->name);

  printf("#<%sprocedure %s ", p->body ? "" : "primitive ", p->name);

  tam_object_print(p->args);

  if (TAMAL_OBJECT_PROCEDURE_PRINT_BODY && p->body)
    {
      printf(" ");
      ASSERT(tam_object_istype_byname(p->body, "pair"));
      tam_object_print(p->body);
    }
  
  printf(">");
}

TAMObject *tam_object_proc_exec ( TAMObject*obj, TAMEnv *env )
{
  TAMObjectProc *p;
  TAMObject *result;

  p = tam_object_data_get (obj, &TAMObjectProcType);

  ASSERT(p->args);
  ASSERT(p->body || p->callback);

  if (p->body)
    {
      TAMObject *tmp;
      ASSERT(!p->callback_data);
      ASSERT(!p->callback);

      tmp = p->body;
      
      ASSERT(!tam_object_istype_byname(tmp, "nil"));

      do
        {
          /* printf("evaluating\n"); */
          result = func_eval(tam_object_pair_car(tmp), env);

          if (tam_env_error_test(env))
            {
              ASSERT(!tam_object_istype_byname(result, "nil"));
              return result;
            }
          
          tmp = tam_object_pair_cdr(tmp);
        }
      while (!tam_object_istype_byname(tmp, "nil"));
    }
  else
    {
      ASSERT(!p->body);
      result = p->callback(p->args, p->callback_data, env);
    }

  return result;
}

void tam_object_proc_setused ( void *data, int use )
{
  TAMObjectProc *p = data;

  ASSERT(data);

  ASSERT(p->args);
  ASSERT(p->body || p->callback);

  (use ? tam_object_used_set : tam_object_used_unset)(p->args);

  if (p->body)
    (use ? tam_object_used_set : tam_object_used_unset)(p->body);
}

TAMObject *tam_object_proc_args_get ( TAMObject *proc )
{
  TAMObjectProc *p;

  ASSERT(proc);
  ASSERT(tam_object_type_match(proc, &TAMObjectProcType));

  p = tam_object_data_get(proc, &TAMObjectProcType);

  ASSERT(p);
  ASSERT(p->args);

  return p->args;
}

int tam_object_proc_formals_validate ( TAMObject *formals, TAMEnv *env, int push_error)
{
  int last_is_point = 0;
  char *error = NULL;
  TAMObject *error_obj = NULL;

  ASSERT(formals);
  ASSERT((!env && !push_error) || env);
  
  
  while (!tam_object_istype_byname(formals, "nil"))
  {
    TAMObject *arg;
    int is_point;

    if(!tam_object_istype_byname(formals, "pair"))
      {
        fprintf(stderr, "Malformed formals. You might be writing a module.\n");
        exit(EXIT_FAILURE);
      }

    arg = tam_object_pair_car(formals);
    formals = tam_object_pair_cdr(formals);

    if (!tam_object_istype_byname(arg, "symbol"))
      {
        error = "Formals must be symbols";
        error_obj = arg;
        break;
      }

    is_point = !strcmp(tam_object_str_get(arg), ".");

    if (last_is_point && is_point)
      {
        error = "Duplicated operator '.'";
        break;
      }
    if (last_is_point && !tam_object_istype_byname(formals, "nil"))
      {
        error = "Extra expression after operator '.'";
        break;
      }
    else
      last_is_point = 0;
    
    if (is_point)
      last_is_point = 1;
  }

  if (last_is_point && !error)
    error = "Missing expression after operator '.'";

  if (error)
    {
      if (push_error)
        tam_env_error_push(env, error, error_obj);
      return 0;
    }

  return 1;
}

int tam_object_proc_body_validate ( TAMObject *body, TAMEnv *env, int push_error)
{
  char *error = NULL;
  TAMObject *error_obj = NULL;

  ASSERT(body);
  ASSERT((!env && !push_error) || env);
  
  
  while (!tam_object_istype_byname(body, "nil"))
    {
      ASSERT(tam_object_istype_byname(body, "pair"));

      if (tam_object_istype_byname(tam_object_pair_car(body), "nil"))
        {
          error = "Unquoted ()";
          break;
        }
      body = tam_object_pair_cdr(body);
    }
  
  if (error)
    {
      if (push_error)
        tam_env_error_push(env, error, error_obj);
      return 0;
    }

  return 1;
}

static const char * tam_object_proc_str ( void * data )
{
  TAMObjectProc *p = data;

  ASSERT(data);

  ASSERT(p->name);

  return p->name;
}
