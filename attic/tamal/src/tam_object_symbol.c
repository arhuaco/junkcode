#include <tamal.h>

typedef struct TAMObjectSymbol TAMObjectSymbol;

struct TAMObjectSymbol
{
  char *symbol;
};

static void            *tam_object_symbol_dup          ( void * );
static void             tam_object_symbol_free         ( void * );
static void             tam_object_symbol_print        ( void * );
static const char      *tam_object_symbol_str          ( void * );

static TAMObjectType TAMObjectSymbolType =
{
  "symbol",
  NULL,
  tam_object_symbol_dup,
  tam_object_symbol_free,
  tam_object_symbol_print,
  NULL,
  tam_object_symbol_str 
};


TAMObject *tam_object_symbol_new ( char *str, TAMEnv *env )
{
  TAMObjectSymbol *new;
 
  ASSERT(str);
  ASSERT(env);

  new = malloc(sizeof(TAMObjectSymbol));
  CHECK_MEM(new);

  new->symbol = strdup(str);
  CHECK_MEM(new->symbol);
  
  return tam_object_new(new, &TAMObjectSymbolType, env);
}

void *tam_object_symbol_dup ( void *data )
{
  TAMObjectSymbol *new;
 
  ASSERT(data);

  new = malloc(sizeof(TAMObjectSymbol));

  CHECK_MEM(new);

  new->symbol = strdup(((TAMObjectSymbol *)data)->symbol);
  CHECK_MEM(new->symbol);
  
  return new; 
}

void tam_object_symbol_free ( void *data )
{
  ASSERT(data);
  ASSERT(((TAMObjectSymbol *)data)->symbol);

  free(((TAMObjectSymbol *)data)->symbol);
  free(data);
}

void tam_object_symbol_print ( void *data )
{
  ASSERT(data);
  ASSERT(((TAMObjectSymbol *)data)->symbol);

  printf("%s", ((TAMObjectSymbol *)data)->symbol);
}

const char *tam_object_symbol_str ( void *data )
{
  ASSERT(data);
  ASSERT(((TAMObjectSymbol *)data)->symbol);

  return ((TAMObjectSymbol *)data)->symbol;
}
