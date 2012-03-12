#include <tamal.h>

typedef struct TAMGarbageList TAMGarbageList;

struct TAMGarbageList
{
  TAMObject *obj;
  TAMGarbageList *next, *prev;
};

struct TAMGarbage
{
  TAMGarbageList *head;
  int size;
};

TAMGarbage *tam_garbage_new ()
{
  TAMGarbage *g;

  g = malloc(sizeof(TAMGarbage));
  CHECK_MEM(g);

  g->size = 0;

  g->head = malloc(sizeof(TAMGarbageList));
  CHECK_MEM(g->head);

  g->head->prev = g->head;
  g->head->next = g->head;

  return g;
}

void tam_garbage_free ( TAMGarbage *g )
{
  ASSERT(g);
  ASSERT(g->head);

  /* TODO: free list */
  free(g);
}

void tam_garbage_register_object ( TAMGarbage *g, TAMObject *obj )
{
  TAMGarbageList *new;

  ASSERT(g);
  ASSERT(g->head);
  ASSERT(g->head->prev);
  ASSERT(g->head->next);
  ASSERT(obj);

  new = malloc(sizeof(TAMGarbageList));
  CHECK_MEM(new);

  new->obj = obj;

  new->prev = g->head->prev;
  new->next = g->head;
  (g->head->prev)->next = new;
  g->head->prev = new;

  g->size ++;
}

void tam_garbage_clear ( TAMGarbage *g )
{
  TAMGarbageList *tmp;

  for (tmp = g->head->next; tmp != g->head; tmp = tmp->next)
    tam_object_used_unset(tmp->obj);
}

#if 0
void garbage_keep (TAMGarbage *g, TAMObject *obj )
{
  tam_object_used_set(obj, 1);
}
#endif

void tam_garbage_sweep( TAMGarbage *g, FILE *p)
{
  TAMGarbageList *tmp;
  int n_deleted = 0;
  int before;

  before = g->size;

  for (tmp = g->head->next; tmp != g->head;)
  {
    if (!tam_object_used_get(tmp->obj))
      {
        TAMGarbageList *d;
        d = tmp;
        tmp = tmp->next;
        (d->prev)->next = d->next;
        (d->next)->prev = d->prev;
        tam_object_free(d->obj);
        free(d);
        g->size --;
        n_deleted ++;
       }
    else
      tmp = tmp->next;
  }
  
  if (TAMAL_GARBAGE_COLLECTION_INFO && p)
    fprintf(p, "[garbage before:%d after:%d deleted:%d]", before, g->size, n_deleted);
}
