#include <tamal.h>

typedef struct TAMObjectPair TAMObjectPair;

struct TAMObjectPair
{
  TAMObject *left;
  TAMObject *right;
};

static void            *tam_object_pair_dup          ( void * );
static void             tam_object_pair_free         ( void * );
static void             tam_object_pair_print        ( void * );
static void             tam_object_pair_setused      ( void *, int use );

static TAMObjectType TAMObjectPairType =
{
  "pair",
  NULL,
  tam_object_pair_dup,
  tam_object_pair_free,
  tam_object_pair_print,
  tam_object_pair_setused,
  NULL
};


TAMObject *tam_object_pair_new ( TAMObject *left,  TAMObject *right, TAMEnv * env)
{
  TAMObjectPair *new;

  ASSERT(env);
  ASSERT(left);
  ASSERT(right);
 
  new = malloc(sizeof(TAMObjectPair));
  CHECK_MEM(new);

  new->left  = tam_object_duplicate(left);
  new->right = tam_object_duplicate(right);
  
  return tam_object_new(new, &TAMObjectPairType, env);
}

void *tam_object_pair_dup ( void *data )
{
  return data; 
}

void tam_object_pair_free ( void *data )
{
  ASSERT(data);

  free(data);
}

/* TODO: this function has legacy code, when we did not have a nil object */
void tam_object_pair_print ( void *data )
{
  TAMObject *l, *r;
  TAMObjectPair *p = data;
  ASSERT(data);

  l = p->left;
  r = p->right;
  
  printf("(");
 
  ASSERT(l);
  ASSERT(r);
  
  while (r && tam_object_type_match(r, &TAMObjectPairType))
    {
      ASSERT(l);
      ASSERT(r);
    
      if (l)
      {
        tam_object_print(l);
        printf(" ");
      }
      else
        printf("NIL . ");

      p = tam_object_data_get (r, &TAMObjectPairType);

      l = p->left;
      r = p->right;
    }

  ASSERT(l);
  ASSERT(r);
  
  if (l)
    tam_object_print(l);
    
  if (r && !tam_object_istype_byname(r, "nil"))
  {
    printf(" . ");
    tam_object_print(r);
  }
  
  printf(")");
}

void tam_object_pair_setused ( void *data, int use )
{
  TAMObject *l, *r;
  TAMObjectPair *p = data;
  ASSERT(data);

  l = p->left;
  r = p->right;

  ASSERT(l);
  ASSERT(r);

  if (l)
    (use ? tam_object_used_set : tam_object_used_unset)(l);

  if (r)
    (use ? tam_object_used_set : tam_object_used_unset)(r);
}

TAMObject *tam_object_pair_car ( TAMObject *pair )
{
  TAMObjectPair *p;

  ASSERT(pair);
  ASSERT(tam_object_type_match(pair, &TAMObjectPairType));

  p = tam_object_data_get(pair, &TAMObjectPairType);

  ASSERT(p);
  ASSERT(p->left);

  return p->left;
}

TAMObject *tam_object_pair_cdr ( TAMObject *pair )
{

  TAMObjectPair *p;

  ASSERT(pair);
  ASSERT(tam_object_type_match(pair, &TAMObjectPairType));

  p = tam_object_data_get(pair, &TAMObjectPairType);

  ASSERT(p);
  ASSERT(p->right);

  return p->right;
}
