#include <tamal.h>
#include <tam_object_module.h>
#include <tam_module.h>

typedef struct TAMObjectModule TAMObjectModule;

struct TAMObjectModule
{
  /* type of the module */
  TAMModuleType    *type;

  /* data of the module of the given type */
  void             *data;
};

static void            *tam_object_module_dup          ( void * );
static void             tam_object_module_free         ( void * );
static void             tam_object_module_print        ( void * );
static void             tam_object_module_setused      ( void *, int use );
static const char      *tam_object_module_getstr       ( void * );

static TAMObjectType TAMObjectModuleType =
{
  "module",
  NULL,
  tam_object_module_dup,
  tam_object_module_free,
  tam_object_module_print,
  tam_object_module_setused,
  tam_object_module_getstr
};

TAMObject *tam_object_module_new ( char *module_name, TAMEnv * env )
{
  TAMObjectModule   *new;

  ASSERT(env);

  new = malloc(sizeof(TAMObjectModule));
  CHECK_MEM(new);

  /*
   * TODO: manage errors without exiting
   */
  
  if (!(new->type = tam_module_type_by_name_get(module_name)))
    {
      fprintf(stderr, "%s:module '%s' not found\n",program_name, module_name);
      exit(EXIT_FAILURE);
    }

  VERBOSE("[loading module:%s]\n", new->type->name);

  ASSERT(new->type->init);
  ASSERT(new->type->free);
  ASSERT(new->type->setused);
  new->data = new->type->init(env);
  ASSERT(new->data);
  
  return tam_object_new(new, &TAMObjectModuleType, env);
}

void *tam_object_module_dup ( void *data )
{
  ASSERT(data);
  return data; 
}

void tam_object_module_free ( void *data )
{
  TAMObjectModule *m = data;
  
  ASSERT(m);
  ASSERT(m->type->free);

  m->type->free(m->data);
  
  free(data);
}

void tam_object_module_print ( void *data )
{
  TAMObjectModule *m = data;
  ASSERT(m);
  ASSERT(m->type);
  ASSERT(m->type->name);

  printf("{module '%s'}", m->type->name);
}

void tam_object_module_setused ( void *data, int use )
{
  TAMObjectModule *m = data;
  ASSERT(m);
  ASSERT(m->type->setused);

  m->type->setused(m->data);
}

static const char *tam_object_module_getstr ( void * data)
{
  ASSERT(data);

  return  "{string of generic module}";
}
