#ifndef __TAM__OBJECT__PROC__
#define __TAM__OBJECT__PROC__

#include <tam_object.h>
#include <tam_primitive.h>

/* create a new object of type procedure */
TAMObject       *tam_object_proc_new          ( TAMObject *args,
                                                TAMObject *body,
                                                TAMEnv    *env      );

/* create a new object from a primitive procedure */
TAMObject       *tam_object_proc_callback_new ( char *name,
                                                TAMObject      *args,
                                                TAMPrimitive   *func,
                                                void           *func_data,
                                                TAMEnv         *env );

/* validates that the formals of the procedure are well formed */
int              tam_object_proc_formals_validate
                                              ( TAMObject *formals,
                                                TAMEnv    *env,
                                                int        push_error );

/* validates that the body of the procedure are well formed */
int              tam_object_proc_body_validate
                                              ( TAMObject *body,
                                                TAMEnv *env,
                                                int push_error);
                                              
/* return the arguments of a procedure */
TAMObject       *tam_object_proc_args_get     ( TAMObject *proc     );

#if 0
/* return the body of a procedure */
TAMObject       *tam_object_proc_body_get     ( TAMObject *proc     );
#endif

/* execute a procedure */
TAMObject       *tam_object_proc_exec         ( TAMObject *proc ,
                                                TAMEnv    *env      );

#endif
