#include <tamal.h>
#include <tam_input.h>

char *program_name = "tamal";

/* add a lot of information when an error occurs */
int tam_arg_errorfull                    =       1;

/* print only the result of the evaluations */
int tam_arg_quiet                        =       0;

int main ( int argc, char *argv[] )
{
  TAMEnv     *env;

  env = tam_environment_new();

  if (!tam_arg_quiet)
    printf(
"\n SCHEMESCHEME  SCHE Tiny\n"
" MESCHEMESC  HEMESC Adhesive\n"
" HEMESCHE  MESCHEME Minimalistic\n"
" SCHEME  SC  HEMESC Anarchic\n"
" HEME  SCHEME  SCHE Lisp\n\n");           
  
  tam_input_parser(env);

  tam_env_free(env);

  printf("\n\nThanks for using %s\n", program_name);

  return EXIT_SUCCESS;
}
