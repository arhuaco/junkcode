#include <tamal.h>

typedef struct TAMObjectBool TAMObjectBool;

struct TAMObjectBool
{
  char value;
  char buff[3];
};

static void            *tam_object_bool_dup          ( void * );
static void             tam_object_bool_free         ( void * );
static void             tam_object_bool_print        ( void * );
static const char      *tam_object_bool_str          ( void * );

static TAMObjectType TAMObjectBoolType =
{
  "bool",
  NULL,
  tam_object_bool_dup,
  tam_object_bool_free,
  tam_object_bool_print,
  NULL,
  tam_object_bool_str 
};

TAMObject *tam_object_bool_new ( char value, TAMEnv *env )
{
  TAMObjectBool *new;
  
  ASSERT(value == 't' || value == 'f');
  ASSERT(env);

  new = malloc(sizeof(TAMObjectBool));
  CHECK_MEM(new);

  new->buff[0] = '#';
  new->buff[1] = value;
  new->buff[2] = '\0';

  new->value = value;
  
  return tam_object_new(new, &TAMObjectBoolType, env);
}

void *tam_object_bool_dup ( void *data )
{
  TAMObjectBool *new;
  char value;

  ASSERT(data);

  value = ((TAMObjectBool *)data)->value;
 
  ASSERT(value == 't' || value == 'f');

  new = malloc(sizeof(TAMObjectBool));

  CHECK_MEM(new);

  new->value = value;
  
  return new; 
}

void tam_object_bool_free ( void *data )
{
  ASSERT(data);

  free(data);
}

void tam_object_bool_print ( void *data )
{
  char value;
  ASSERT(data);

  value = ((TAMObjectBool *)data)->value;

  ASSERT(value == 't' || value == 'f');

  printf("#%c", value);
}

const char *tam_object_bool_str ( void *data )
{
  TAMObjectBool *val;

  val = ((TAMObjectBool *)data);
  
  return val->buff;
}

int tam_object_bool_istrue( TAMObject *obj )
{
  TAMObjectBool *b;

  ASSERT(obj);

  b = tam_object_data_get(obj, &TAMObjectBoolType);
  ASSERT(b);
  ASSERT(b->value == 't' || b->value == 'f');

  return b->value == 't';
}
