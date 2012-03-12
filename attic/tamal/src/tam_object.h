#ifndef __TAM__OBJECT__
#define __TAM__OBJECT__

typedef struct TAMObject TAMObject;

typedef struct TAMObjectType TAMObjectType;

#include <tam_garbage.h>

/* duplicate an object. If the object returns the same value (data), then we
 * will have to manage reference counts */
typedef void *(TAMObjectDup)          ( void *data );

/* free an object */
typedef void  (TAMObjectFree)         ( void *data );

/* print the contents of the object */
typedef void  (TAMObjectPrint)        ( void *data );

/* important for garbage collection of compound data */
typedef void  (TAMObjectSetUsed)      ( void *data, int use );

/*  string with the data of the object, if it makes sense */
typedef const char *(TAMObjectStrGet) ( void *data );

struct TAMObjectType
{
  char                  *name;
  void                  *data;  /* static data used by objects */

  TAMObjectDup          *dup;
  TAMObjectFree         *free;
  TAMObjectPrint        *print;
  TAMObjectSetUsed      *setused; /* optional. Might be NULL */
  TAMObjectStrGet       *strget;  /* optional, Might be NULL */
};

/* create a new object of a given type */
TAMObject   *tam_object_new                   ( void              *data,
                                                TAMObjectType     *type,
                                                TAMEnv            *env    );
/* free an object */
void         tam_object_free                  ( TAMObject         *obj   );

/* print the object to stdout */
void         tam_object_print                 ( TAMObject         *obj   );

/* duplicate the object */
TAMObject   *tam_object_duplicate             ( TAMObject         *obj   );

/* says whether an object is of a given type */
int          tam_object_type_match          ( TAMObject         *obj,
                                              TAMObjectType     *type    );

/* says whether an object is of a given type or not, using th type name */
int          tam_object_istype_byname       ( TAMObject         *obj,
                                              const char        *name    );

/* We will only give the data to a object that knows the type */
void        *tam_object_data_get            ( TAMObject         *obj,
                                              TAMObjectType     *type    );
/* set the used mark to true */
void         tam_object_used_set            ( TAMObject         *obj     );

void         tam_object_used_unset          ( TAMObject         *obj     );

/* get the used mark */
int          tam_object_used_get            ( TAMObject        *obj      );

/* is there a string associated with this object? */
int          tam_object_str_can_get         ( TAMObject        *obj      );

/* get the string associated to the object, if any */
const char  *tam_object_str_get             ( TAMObject        *obj      );

/* get a string with the name of the type of the object */
const char  *tam_object_type_name_get       ( TAMObject        *obj      );

#endif
