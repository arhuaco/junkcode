#include <tamal.h>

static void            *tam_object_undef_dup          ( void * );
static void             tam_object_undef_free         ( void * );
static void             tam_object_undef_print        ( void * );

/* This value must be different from 0. A prime number is ok =) */
#define DUMMY_UNDEF_VALUE 71

static TAMObjectType TAMObjectUndefType =
{
  "undef",
  NULL,
  tam_object_undef_dup,
  tam_object_undef_free,
  tam_object_undef_print,
  NULL,
  NULL
};

TAMObject *tam_object_undef_new ( TAMEnv *env )
{
  return tam_object_new((void*)DUMMY_UNDEF_VALUE, &TAMObjectUndefType, env);
}

void *tam_object_undef_dup ( void *data )
{
  ASSERT((int)data == DUMMY_UNDEF_VALUE);

  return data; 
}

void tam_object_undef_free ( void *data )
{
  ASSERT((int)data == DUMMY_UNDEF_VALUE);
}

void tam_object_undef_print ( void *data )
{
  ASSERT((int)data == DUMMY_UNDEF_VALUE);

  printf("#unspecified");
}
