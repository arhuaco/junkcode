#include <tamal.h>

typedef struct TAMObjectNumber TAMObjectNumber;

struct TAMObjectNumber
{
  double number;
};

static void            *tam_object_number_dup          ( void * );
static void             tam_object_number_free         ( void * );
static void             tam_object_number_print        ( void * );

static TAMObjectType TAMObjectNumberType =
{
  "number",
  NULL,
  tam_object_number_dup,
  tam_object_number_free,
  tam_object_number_print,
  NULL,
  NULL
};


TAMObject *tam_object_number_dbl_new ( double number, TAMEnv *env )
{
  TAMObjectNumber *new;
 
  ASSERT(env);

  new = malloc(sizeof(TAMObjectNumber));
  CHECK_MEM(new);
  new->number = number;
  
  return tam_object_new(new, &TAMObjectNumberType, env);
}

TAMObject *tam_object_number_new ( char *str, TAMEnv *env)
{
  char *end;
  TAMObjectNumber *new;
  double number;
 
  ASSERT(str);
  ASSERT(env);

  number = strtod(str, &end);

  if(*end)
  {
    fprintf(stderr, "%s : invalid number : '%s'\n", program_name, str);
    return NULL;
  }

  new = malloc(sizeof(TAMObjectNumber));
  CHECK_MEM(new);
  new->number = number;
  
  return tam_object_new(new, &TAMObjectNumberType, env);
}

void *tam_object_number_dup ( void *data )
{
  TAMObjectNumber *new;
 
  ASSERT(data);

  new = malloc(sizeof(TAMObjectNumber));

  CHECK_MEM(new);

  new->number = ((TAMObjectNumber *)data)->number;
  
  return new; 
}

void tam_object_number_free ( void *data )
{
  ASSERT(data);

  free(data);
}

void tam_object_number_print ( void *data )
{
  TAMObjectNumber *n;
  int x;
  ASSERT(data);
 
  n = data;

  x = (int)n->number;

  if ((double)x == n->number)
    printf("%d", x);
  else
    printf("%.8g", n->number);
}

double tam_object_number_value_get ( TAMObject *obj )
{
  TAMObjectNumber *n;

  ASSERT(obj);

  n = tam_object_data_get(obj, &TAMObjectNumberType);
  ASSERT(n);

  return n->number; 
}
  
