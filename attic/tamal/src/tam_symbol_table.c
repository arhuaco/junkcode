#include <tamal.h>
#include <tam_symbol_table.h>
#include <tam_hashfunc.h>

/*
 * Knuth : Early optimization is the root of all evil.
 * I should not have used a hash, yet.
 */

typedef struct TAMSymbolEntry TAMSymbolEntry;

struct TAMSymbolEntry
{
  /* name of the symbol */
  TAMObject *name;

  /* value of the symbol */
  TAMObject *value;

  /* next entry in the hash bucket */
  TAMSymbolEntry *next;
};

struct TAMSymbolTable
{
  /* size of the hash table we'll use */
  unsigned int hash_size;

  /* hash table */
  TAMSymbolEntry **table;
  
  /* next symbol table (previous level) */
  TAMSymbolTable *next;
};

TAMSymbolTable *tam_symbol_table_new ( unsigned int hash_size )
{
  int i;
  TAMSymbolTable *t;

  ASSERT(hash_size >= 1 && hash_size <= 32768);

  t = malloc(sizeof(TAMSymbolTable));
  CHECK_MEM(t);

  t->hash_size = hash_size;
  t->next = NULL;

  t->table = malloc(sizeof(TAMSymbolEntry *) * hash_size);
  CHECK_MEM(t->table);

  for (i = 0; i < hash_size; ++i)
    t->table[i] = NULL;

  return t;
}

void tam_symbol_table_free ( TAMSymbolTable *st )
{
  int i;
  TAMSymbolEntry *tmp, *d;

  ASSERT(st);
  ASSERT(st->table);
  ASSERT(st->hash_size > 0);

  while (st->next)
    tam_symbol_table_shrink(st);

  for (i = 0; i < st->hash_size; ++i)
    for (tmp = st->table[i]; tmp; )
      {
        d = tmp;
        tmp = tmp->next;
        free(d);
      }
}

void tam_symbol_table_extend ( TAMSymbolTable *st )
{
  TAMSymbolTable *new;
  TAMSymbolEntry **tmp;

  ASSERT(st);
  ASSERT(st->hash_size);
  ASSERT(st->table);

  new = tam_symbol_table_new(st->hash_size);

  /* isn't this ugly? */
  tmp = new->table;
  new->table = st->table;
  st->table = tmp;

  new->next = st->next;
  st->next = new;
}

void tam_symbol_table_shrink ( TAMSymbolTable *st )
{
  TAMSymbolEntry **tmp;
  TAMSymbolTable *old;

  ASSERT(st);
  ASSERT(st->table);
  ASSERT(st->next); /* in order to skrink the table needs more that 1 level */

  /* isn't this ugly? */
  old = st->next;
  tmp = old->table;
  old->table = st->table;
  st->table = tmp;

  st->next = old->next;
  old->next = NULL;
  tam_symbol_table_free(old);
}

static TAMSymbolEntry *
seek_in_bucket (TAMSymbolTable *st, unsigned int bucket_pos,
                                    const char *name_str)
{
  TAMSymbolEntry *tmp;

  ASSERT(st);
  ASSERT(name_str);

  for (tmp = st->table[bucket_pos]; tmp; tmp = tmp->next)
    if (!strcmp(name_str, tam_object_str_get(tmp->name)))
      return tmp;

  return NULL;
}

int tam_symbol_table_add ( TAMSymbolTable *st, TAMObject *name,
                                               TAMObject *value,
                                               int overwrite )
{
  unsigned int bucket_pos;
  const char *name_str;
  TAMSymbolEntry *found, *new;

  ASSERT(st);
  ASSERT(name);
  ASSERT(value);
  
  /* we check that the symbol is not already defined in the current level */

  if (!tam_object_istype_byname(name, "symbol"))
  {
    fprintf(stderr, "%s:cannot add object to the symbol table\n", program_name);
    exit(1);
  }
  
  name_str = tam_object_str_get(name);
  
  bucket_pos = string_hash(name_str) % st->hash_size;

  found = seek_in_bucket(st, bucket_pos, name_str);

  if (found && !overwrite)
    return 0;

  if (found)
    {
      found->name = tam_object_duplicate(name);
      found->value = tam_object_duplicate(value);
      return 1;
    }

  new = malloc(sizeof(TAMSymbolEntry));
  CHECK_MEM(new);

  new->name  = tam_object_duplicate(name);
  new->value = tam_object_duplicate(value);
  new->next  = st->table[bucket_pos];
  st->table[bucket_pos] = new;

  return 1;
}

int tam_symbol_table_query ( TAMSymbolTable *st, TAMObject *name, TAMObject **value )
{
  unsigned int bucket_pos;
  const char *name_str;
  TAMSymbolEntry *found = NULL;
  TAMSymbolTable *level;

  ASSERT(st);
  ASSERT(name);
  ASSERT(value);
  
  if (!tam_object_istype_byname(name, "symbol"))
  {
    fprintf(stderr, "%s:cannot query the symbol table with an object of this type\n", program_name);
    exit(1);
  }
  
  name_str = tam_object_str_get(name);
  
  bucket_pos = string_hash(name_str) % st->hash_size;

  for (level = st; level && !found; level = level->next)
    found = seek_in_bucket(level, bucket_pos, name_str);

  if (found)
    {
      *value = found->value;
      return 1;
    }

  return 0;
}

void tam_symbol_table_used_set ( TAMSymbolTable *st )
{
  TAMSymbolEntry *tmp;
  int i;

  ASSERT(st);
  ASSERT(st->table);
  
  for ( ; st; st = st->next)
    for (i = 0; i < st->hash_size; ++i)
      for (tmp = st->table[i]; tmp; tmp = tmp->next)
        {
          if (tmp->name)
            tam_object_used_set(tmp->name);

          if (tmp->value)
            tam_object_used_set(tmp->value);
        }
}
