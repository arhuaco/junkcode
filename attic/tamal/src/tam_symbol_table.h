#ifndef __SYMBOL__TABLE__
#define __SYMBOL__TABLE__

typedef struct TAMSymbolTable TAMSymbolTable;

#include <tamal.h>

/* create a symbol table. hash_size should be a prime number */
TAMSymbolTable      *tam_symbol_table_new           ( unsigned int hash_size );

/* free a symbol table */
void                 tam_symbol_table_free          ( TAMSymbolTable *st     );

/* extend the symbol table one level */
void                 tam_symbol_table_extend        ( TAMSymbolTable *st     );

/* shrink the symbol table one level */
void                 tam_symbol_table_shrink        ( TAMSymbolTable *st     );

/* add a symbol to the symbol table, in the current level.
 * return non-zero on success */
int                  tam_symbol_table_add           ( TAMSymbolTable *st,
                                                      TAMObject      *name,
                                                      TAMObject      *value,
                                                      int             overwrite                                                                             );
/* query for a symbol in all levels, it will return a new object
 * that will be garbage-collected. Never try to free it yourself.
 * Will return non-zero on success */
int                  tam_symbol_table_query        ( TAMSymbolTable *st,
                                                     TAMObject      *name,
                                                     TAMObject     **value );

/* mark the used object as such so they're not garbage collected */
void                 tam_symbol_table_used_set     ( TAMSymbolTable *st );

#endif
