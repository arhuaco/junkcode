#ifndef __TAM__STACK__
#define __TAM__STACK__

typedef struct TAMStack TAMStack;

/* create an empty stack */
TAMStack             *tam_stack_new           ( void );

/* free a stack */
void                  tam_stack_free          ( TAMStack * );

/* test if the stack is empty */
int                   tam_stack_isempty       ( TAMStack * );

/* push an object into the stack */
void                  tam_stack_push          ( TAMStack *s, void *obj, int type );

/* pop an object from the stack. Either obj or type can be optional */
void                  tam_stack_pop           ( TAMStack *s, void **obj, int *type );

#endif
