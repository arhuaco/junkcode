#include <tamal.h>
#include <tam_error.h>

typedef struct TAMErrorData TAMErrorData;

struct TAMErrorData{
  /* string describing errors */
  char             *msg;

  /* next error in stack */
  TAMErrorData     *next;
};

struct TAMError
{
  /* error stack */
  TAMErrorData     *errors;
};

TAMError *tam_error_new ()
{
  TAMError *e;

  e = malloc(sizeof(TAMError));
  CHECK_MEM(e);

  e->errors = NULL;

  return e;
}

void tam_error_free ( TAMError *e )
{
  ASSERT(e);

  if (e->errors);
    tam_error_flush(e);

  free(e);
}

int tam_error_isset ( TAMError *e )
{
  ASSERT(e);

  return e->errors != NULL;
}

void tam_error_push ( TAMError *e, char *msg )
{
  TAMErrorData *d;

  ASSERT(e);
  ASSERT(msg);

  d = malloc(sizeof(TAMErrorData));
  CHECK_MEM(d);

  d->msg = strdup(msg);
  CHECK_MEM(d->msg);

  d->next = e->errors;
  e->errors = d;
}

void tam_error_print ( TAMError *e, FILE *f )
{
  TAMErrorData *info;
  
  ASSERT(e);
  ASSERT(f);

  for (info = e->errors; info; info = info->next)
    {
      ASSERT(info->msg);

      fprintf(f, "Error : %s\n", info->msg);
    }
}

void tam_error_flush ( TAMError *e )
{
  TAMErrorData *info, *d;

  ASSERT(e);

  for (info = e->errors; info; )
    {
      d = info;
      info = info->next; 

      ASSERT(d->msg);
      
      free(d->msg);
      free(d);
    }

  e->errors = NULL;
}
