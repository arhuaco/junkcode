#ifndef __TAM__AUXEVAL__H__
#define __TAM__AUXEVAL__H__

#include <tamal.h>

/*
 * Functions to be used internally for misc. evaluations
 */

/* is this a list with a known tag? */
int                  is_tagged_list      (  TAMObject *exp,
                                            char *tag,
                                            TAMEnv *env            );

/* ckeck string lengths with an optional limit */
unsigned int         list_length         (  TAMObject *obj,
                                            TAMEnv *env,
                                            unsigned int max       );
#endif
