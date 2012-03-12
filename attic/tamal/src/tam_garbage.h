#ifndef __TAM__GARBAGE__
#define __TAM__GARBAGE__

#include <stdio.h>

typedef struct TAMGarbage TAMGarbage;

#include <tam_object.h>

/* create a garbage collector */
TAMGarbage       *tam_garbage_new                ( void );

/* free a garbage collector */
void              tam_garbage_free               ( TAMGarbage * );

/* register an object with the garbage collector */
void              tam_garbage_register_object    ( TAMGarbage *g,
                                                   TAMObject *obj );

/* clear the 'used' flag of all the objects in tha database */
void              tam_garbage_clear              ( TAMGarbage * );

/* delete all the unused objects. Optionally, print a summary
 * with the result of the operations */  
void              tam_garbage_sweep              ( TAMGarbage *, FILE *);
  
#endif
