#ifndef __TAM__INPUT__
#define __TAM__INPUT__

/* start the parser. */

#include <tam_garbage.h> 
#include <tam_environment.h>

/* start an interpreter with a given execution environment.
 * This function is not thread safe, but it could be with
 * just a few minor modifications */

int                    tam_input_parser          ( TAMEnv       *env );
  
#endif
