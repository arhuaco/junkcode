#ifndef __TAM__OBJECT__PAIR__
#define __TAM__OBJECT__PAIR__

#include <tam_object.h>

/* create a new object of type pair */
TAMObject *tam_object_pair_new                ( TAMObject *left,
                                                TAMObject *right,
                                                TAMEnv    *env      );

/* (car object) : return the left part */
TAMObject *tam_object_pair_car                ( TAMObject *pair     );

/* (cdr object) : return the right part */
TAMObject *tam_object_pair_cdr                ( TAMObject *pair     );

#endif
