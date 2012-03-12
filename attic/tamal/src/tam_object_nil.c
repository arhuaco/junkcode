#include <tamal.h>

static void            *tam_object_nil_dup          ( void * );
static void             tam_object_nil_free         ( void * );
static void             tam_object_nil_print        ( void * );
static const char      *tam_object_nil_strget       ( void * );

/* This value must be different from 0. A perfect number is ok =) */
#define DUMMY_NIL_VALUE 28

static TAMObjectType TAMObjectNilType =
{
  "nil",
  NULL,
  tam_object_nil_dup,
  tam_object_nil_free,
  tam_object_nil_print,
  NULL,
  tam_object_nil_strget
};

TAMObject *tam_object_nil_new ( TAMEnv *env )
{
  return tam_object_new((void*)DUMMY_NIL_VALUE, &TAMObjectNilType, env);
}

void *tam_object_nil_dup ( void *data )
{
  ASSERT((int)data == DUMMY_NIL_VALUE);

  return data; 
}

void tam_object_nil_free ( void *data )
{
  ASSERT((int)data == DUMMY_NIL_VALUE);
}

void tam_object_nil_print ( void *data )
{
  ASSERT((int)data == DUMMY_NIL_VALUE);

  printf("()");
}

const char *tam_object_nil_strget ( void *data )
{
  ASSERT((int)data == DUMMY_NIL_VALUE);

  return "()";
}

