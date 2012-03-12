%{
  #define YYSTYPE char *

  #include <stdio.h>
  #include <ctype.h>
  #include <tamal.h>

  /* execution environment */
  TAMEnv       *ENV;

  #define PVERBOSE printf

  /* Stack of Objects */
  TAMStack *stack;

  /* Add an object to the stack */
  static int   add_to_stack  ( int type, char *str );
  
  /* Remove all objects from the Stack */
  static void  empty_stack   ( void );

  /* silly prompt */
  static void show_prompt    (void)
  {
     printf ("%s> ", program_name);
  }

  /* If we can parse the input, then we can use this function to convert
   * data on the stack on code. Note that with "code" I mean, DATA that
   * will be arranged in lists that, in turn, will be evaluated later. */
  static TAMObject *from_stack_to_code ( void );
%}

%token   NUMBER
%token   STRING
%token   SYMBOL

%%
 input:  /* empty string */
        | input line
        ;
 line:    '\n'
        | exp   {
                TAMObject *code = from_stack_to_code();
                ASSERT(code);
                tam_env_exec(ENV, code);
                show_prompt();
                }
        | error { empty_stack(); }
      ;

 exp:     NUMBER            { add_to_stack(NUMBER, $1);          } 
        | STRING            { PVERBOSE ("STRING\n"); /*not used*/}
        | SYMBOL            { add_to_stack(SYMBOL, $1);          }
        | open exp exps close 
        | quote exp          
        | open close        /* we will check that it's quoted in eval */
      ;

 quote:   '\''              { add_to_stack('\'', NULL);          }
      ;

 open:    '('               { add_to_stack('(', NULL);           }
      ;

 close:   ')'               { add_to_stack(')', NULL);           }
      ;

 exps:   /* empty string */
        | exp exps
      ;

%%

int add_to_stack ( int type, char *str )
{
  TAMObject *obj = NULL;

  switch (type)
    {
      case NUMBER:
        obj = tam_object_number_new(str, ENV);
        break;

      case SYMBOL:
        obj = tam_object_symbol_new(str, ENV);
        break;

      default:
        ASSERT(type == '(' || type == ')' || type == '\'');
        tam_stack_push(stack, NULL, type);
        return 0;
    }

  ASSERT(obj);

  ASSERT(type == NUMBER || type == SYMBOL);
  tam_stack_push(stack, obj, type);

  return 0;
}  

void empty_stack ()
{
  while (!tam_stack_isempty(stack))
  {
    TAMObject *obj;
    int type;

    tam_stack_pop (stack, (void*)&obj, &type); 

    if (obj)
    {
      printf("discarding \"");
      tam_object_print(obj);
      printf("\"\n");
      tam_object_free(obj);
    }
  }
}

static char *lex_buffer = NULL;
static int   lex_buffer_size = 32;
static int   lex_len;

static void buffer_init (void)
{
  lex_len = 0;
}

static void buffer_add (char c)
{
  if (lex_len + 2 > lex_buffer_size)
    {
      lex_buffer_size += 32;
      lex_buffer = realloc(lex_buffer, lex_buffer_size);
      CHECK_MEM(lex_buffer);
    }
  lex_buffer[lex_len++] = c;
}

static char *buffer_end(void)
{
  ASSERT(lex_len < lex_buffer_size);

  lex_buffer[lex_len] = '\0';
  return lex_buffer;
}

yylex ()
{
  int c;

  buffer_init();

  /* skip white spaces  */
  while (isspace((c = getchar())))
    ;

  /* quotations ... */
  if (c == '\'')
    return c;

  if (c == ')' || c=='(')
    return c;

  if (c == EOF)
    return 0;

  /* symbols or numbers */
  if (!isspace(c) && c != '(' && c != ')')
    {
       int number_have_point = 0;
       int parse_as_symbol = 0;

       while (!isspace(c) && c != '(' && c != ')')
        {
          if (!parse_as_symbol)
            {
              if (c != '.' && !isdigit(c))
                parse_as_symbol = 1;
              else if (c == '.')
                {
                  if(number_have_point)
                    parse_as_symbol = 1;

                  number_have_point = 1;
                }
            }

          buffer_add(c);
          c = getchar();
        }

      ungetc(c, stdin);
      yylval = buffer_end();

      if (!strcmp(yylval, "."))
        return SYMBOL;

      return parse_as_symbol ? SYMBOL : NUMBER;
    }

  fprintf(stderr, "%s : invalid input '%c' (%d). please email the author\n",
                  program_name, c, c);
  exit(EXIT_FAILURE);
}

int tam_input_parser ( TAMEnv *env )
{
  lex_buffer = malloc(lex_buffer_size);
  CHECK_MEM(lex_buffer);

  stack = tam_stack_new();

  show_prompt();

  ENV  = env;

  yyparse ();

  empty_stack();
  tam_stack_free(stack);
  free(lex_buffer);

  return 0;
}

yyerror (s) 
char *s;
{
 fprintf (stderr, "%s : '%s'. Rock on...\n", program_name, s);
}

TAMObject *from_stack_to_code ( void )
{
  int done = 0;
  TAMObject *first;

  first = tam_env_nil_get(ENV);

  while (!done && !tam_stack_isempty(stack))
    {
      TAMObject *new;
      int type;

      ASSERT(!tam_stack_isempty(stack));

      tam_stack_pop(stack, (void *)&new, &type);

      if (type == '\'')
        {
          ASSERT(!new);
          ASSERT(tam_object_istype_byname(first, "pair"));

          new = tam_object_pair_new(tam_env_quote_get(ENV),
                                    tam_object_pair_new(
                                       tam_object_pair_car(first),
                                       tam_env_nil_get(ENV), ENV),
                                    ENV);

          first = tam_object_pair_cdr(first);
        }
      else if (type == '(')
        {
          ASSERT(!new);
          done = 1;
        }
      else if (type == ')')
        {
          ASSERT(!new);
          new = from_stack_to_code();
        }

      if (!done)
        {
          ASSERT(new);
          ASSERT(first);

          first = tam_object_pair_new(new, first, ENV);
        }
    }

  return first;
}
