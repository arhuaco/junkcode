#include <tamal.h>

struct TAMObject
{
  /* Data of the Object */
  void            *data;
 
  /* Type of the object. Includes the methods to operate on it */ 
  TAMObjectType   *type;

  /* references for the garbage collector */
  char used;

  /* garbage collector : "owner" of this object */
  TAMEnv *env;
};

TAMObject *tam_object_new ( void *data, TAMObjectType *type, TAMEnv *env)
{
  TAMObject * obj;

  ASSERT(data);
  ASSERT(type);
  ASSERT(env);

  obj = malloc(sizeof(TAMObject));

  CHECK_MEM(obj);

  obj->data     = data;
  obj->type     = type;
  obj->env      = env;

  /* allow garbage collection */
  tam_env_register_obj (obj->env, obj); 

  return obj;
}

void tam_object_free ( TAMObject *obj )
{
  ASSERT(obj);

#if 0
  printf("\nI will free an object of type %s\n", obj->type->name);
#endif

  obj->type->free(obj->data);
  free(obj);
}

void tam_object_print ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->type);
  ASSERT(obj->type->print);

  obj->type->print(obj->data);
}

TAMObject *tam_object_duplicate ( TAMObject *obj )
{
  void *dup;
  ASSERT(obj);
  ASSERT(obj->type);
  ASSERT(obj->data);
  ASSERT(obj->type->dup);


  if (!TAMAL_ALLOW_ASSIGNATIONS)
    return obj;

  dup = obj->type->dup(obj->data);

  if (dup == obj->data)
    {
      return obj;
    }

  return tam_object_new(dup, obj->type, obj->env);
}

int tam_object_type_match ( TAMObject *obj, TAMObjectType *type)
{
  ASSERT(obj);

  return obj->type == type;
} 

void *tam_object_data_get ( TAMObject *obj, TAMObjectType *type)
{
  ASSERT(obj);

  if (obj->type != type)
    {
      fprintf(stderr, "%s:tam_object_data_get:invalid request\n", program_name);
      exit(EXIT_FAILURE);
    }
  
  return obj->data;
}

TAMEnv *tam_environment_get ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->env);

  return obj->env;
}

void tam_object_used_unset ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);
   
  if (!obj->used)
    return;
  
  obj->used = 0;

  if (obj->type->setused)
    obj->type->setused(obj->data, 0);
}

void tam_object_used_set ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);

  if (obj->used)
    return;

  obj->used = 1;

  if (obj->type->setused)
    obj->type->setused(obj->data, 1);
}

int tam_object_used_get ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);
  
  return obj->used;
}

int tam_object_istype_byname ( TAMObject *obj, const char *name)
{
  ASSERT(obj); 
  ASSERT(obj->type);
  ASSERT(obj->type->name);
  ASSERT(name);

  return !strcmp(obj->type->name, name);
}

const char *tam_object_str_get ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);
  ASSERT(obj->type);
  ASSERT(obj->type->strget);

  return obj->type->strget(obj->data);
}

int tam_object_str_can_get ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);
  ASSERT(obj->type);
  
  return obj->type->strget != NULL;
}

const char *tam_object_type_name_get ( TAMObject *obj )
{
  ASSERT(obj);
  ASSERT(obj->data);
  ASSERT(obj->type);
  ASSERT(obj->type->name);

  return obj->type->name;
}
