#ifndef __TAMAL__H__
#define __TAMAL__H__

#include <tam_environment.h>
#include <tam_garbage.h>
#include <tam_object.h>
#include <tam_object_bool.h>
#include <tam_object_nil.h>
#include <tam_object_number.h>
#include <tam_object_module.h>
#include <tam_object_pair.h>
#include <tam_object_proc.h>
#include <tam_object_symbol.h>
#include <tam_object_undef.h>

#include <tam_stack.h>
#include <tam_symbol_table.h>

#include <tam_primitive.h>
#include <tam_module.h>
#include <tam_module_lispcore.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(x) \
  if(!(x)) \
      fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), \
      exit(EXIT_FAILURE)

#define CHECK_MEM(x) \
  if(!(x)) \
      fprintf(stderr, "%s:out of memory\n", program_name), exit(EXIT_FAILURE)

#define VERBOSE printf

#define INLINE inline

/* the name of the program */
extern char *program_name;

extern int tam_arg_errorfull;

/* size of the hash-table of the symbols table at each level */
#define TAMAL_SYMBOLTBL_LEVEL_HASH_SIZE  2

/* should we print the body of the procedures, too? */
#define TAMAL_OBJECT_PROCEDURE_PRINT_BODY 1

/* should we print the data before evaluating it? */
#define TAMAL_ENV_PRINT_BEFORE_EVALUATING 1

/* We might allow asignations in the future... if so, we might have to
 * change a few calls to tam_object_duplicate... */
#define TAMAL_ALLOW_ASSIGNATIONS 0

/* shall we whow the garbage collection info? */
#define TAMAL_GARBAGE_COLLECTION_INFO 1

#endif
