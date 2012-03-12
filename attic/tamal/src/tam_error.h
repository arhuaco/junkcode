#ifndef __TAM__ERROR__
#define __TAM__ERROR__

/*
 * Manage an error stack.
 */

typedef struct TAMError TAMError;

#include <tamal.h>

/* create the error satack */
TAMError               *tam_error_new      ( void );

/* delete the error stack */
void                    tam_error_free     ( TAMError *e );

/* is there at least one error in the stack? */
int                     tam_error_isset    ( TAMError *e );

/* push an error onto the stack */
void                    tam_error_push     ( TAMError *e, char *msg );

/* print the error stack */
void                    tam_error_print    ( TAMError *e, FILE *f );

/* empty the error stack */
void                    tam_error_flush    ( TAMError *e );
  
#endif

