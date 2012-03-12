#ifndef __TAM__EXEC__H__
#define __TAM__EXEC__H__

/*
 * Data for a execution environment
 */

typedef struct TAMEnv TAMEnv;

#include <tam_object.h>


/* create an execution environment */
TAMEnv       *tam_environment_new   ( void );

/* free an execution environment */
void          tam_env_free          ( TAMEnv *env );

/* execute code within an environment */
void          tam_env_exec          ( TAMEnv *env, TAMObject *code );

/* add a symbol to the current context in the symbol table of the env.
 * return non-zero on success */
int           tam_env_symbol_add    ( TAMEnv *env,
                                      TAMObject *name,
                                      TAMObject *value,
                                      int overwrite );

/* lookup a symbol. return the Object is found, NULL is not found */
TAMObject    *tam_env_symbol_lookup ( TAMEnv *env, TAMObject *name );

/* add a frame to the symbol table */
void          tam_env_extend        (TAMEnv *env);

/* delete a frame from the symbol table */
void          tam_env_shrink        (TAMEnv *env);

/* perform garbage collection on this environment */
void          tam_env_sweep         ( TAMEnv *env );

/* register an object within the environment : for garbage collection */
void          tam_env_register_obj  ( TAMEnv *env, TAMObject *obj );

/* push an error into the environment, with a message and an optional
 * object */
void          tam_env_error_push    ( TAMEnv *env, char *msg, TAMObject *obj );

/* are we in a error state? */
int           tam_env_error_test    ( TAMEnv *env );

/* dump error information */
void          tam_env_error_print   ( TAMEnv *env, FILE   *f );  

/* clear the error state */
void          tam_env_error_flush   ( TAMEnv *env );

/* get a reference of a shared object of type "nil" */
TAMObject    *tam_env_nil_get      ( TAMEnv *env );

/* get a reference of a shared object of type "undef" */
TAMObject    *tam_env_undef_get      ( TAMEnv *env );

/* get a reference of a shared object of type bool value #t  */
TAMObject    *tam_env_true_get      ( TAMEnv *env );

/* get a reference of a shared object of type bool value #f */
TAMObject    *tam_env_false_get      ( TAMEnv *env );

/* get a reference of a shared object of type symbol value "." */
TAMObject    *tam_env_dot_get        ( TAMEnv *env );

/* get a reference of a shared object symbol with value "quote" */
TAMObject    *tam_env_quote_get      ( TAMEnv *env );

#endif
