#include <tamal.h>
#include <tam_stack.h>

struct TAMStack
{
  void          *obj;   /* object into the stack */
  int            type;  /* object type : managed by the user */
  TAMStack      *prev;  /* previous object in the stack */
};

TAMStack *tam_stack_new ()
{
  TAMStack *new;

  new = malloc(sizeof(TAMStack));

  CHECK_MEM(new);

  new->prev = NULL;

  return new;
}

void tam_stack_free ( TAMStack *s )
{
  ASSERT(s);
  ASSERT(!s->prev); /* the stack must be empty */

  free(s);
}

int tam_stack_isempty ( TAMStack *s )
{
  ASSERT(s);

  return s->prev == NULL;
}

void tam_stack_push ( TAMStack *s, void *obj, int type )
{
  TAMStack *new;

  ASSERT(s);

  new = malloc(sizeof(TAMStack));

  CHECK_MEM(new);

  new->prev = s->prev;
  s->prev = new;

  new->obj  = obj;
  new->type = type;
}
  

void tam_stack_pop ( TAMStack *s, void **obj, int *type )
{
  TAMStack *old;

  ASSERT(s);
  ASSERT(s->prev); /* stack cannot be empty */
  ASSERT(obj || type);

  old       = s->prev;
  s->prev   = old->prev;

  if (obj)
    *obj = old->obj;

  if (type)
    *type = old->type;

  free(old);
}
