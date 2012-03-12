/*
 * A tail-like program that will print the last lines read
 * from stdin or from a file.
 */

#include <stdio.h>
#include <stdlib.h>

/* First part, here we deal with the stdin part. We make the file
 * part in a different way */

typedef struct Line
{
  char *buf;
  int size;
  int max_size;
} Line;

Line *
line_new (void)
{
  Line *n;

  n = malloc(sizeof(Line));

  if (!n)
  {
    fprintf(stderr, "Error in line_new\n");
    exit(EXIT_FAILURE);
  }

  n->size = 0;
  n->max_size = 8;

  n->buf = malloc(n->max_size);

  if (!n->buf)
  {
    free(n);
    fprintf(stderr, "Error in line_new\n");
    exit(EXIT_FAILURE);
  }

  return n;
}

void
line_grow (Line *l)
{
  l->max_size *= 2;
  l->buf = realloc(l->buf, l->max_size);

  if (!l->buf)
  {
    fprintf(stderr, "Error in line_grow\n");
    exit(EXIT_FAILURE);
  }
}

void
line_compact (Line *l)
{
  if (l->size == l->max_size)
    return;

  l->buf = realloc(l->buf, l->size);

  if (!l->buf)
  {
    fprintf(stderr, "Error in line_compact\n");
    exit(EXIT_FAILURE);
  }

  l->max_size = l->size;
}

void
line_free (Line *l)
{
  if (l)
    free(l->buf);
  free(l);
}

/* not very efficient but simple */
Line *
io_readline (FILE *f)
{
  Line *l;
  int c;

  l = line_new();

  while (!feof(f))
  {
    c = fgetc(f);

    if (c == EOF || c == '\n')
      break;

    if (l->size >= l->max_size)
      line_grow(l);

    l->buf[l->size++] = c;
  }

  if (!l->size && feof(f))
  {
    line_free(l);
    return NULL;
  }

  if (l->size)
    line_compact(l);
  else
  {
    free(l->buf);
    l->buf = NULL;
  }

  return l;
}

void
tail_reading_all(int N, FILE *stream)
{
  Line **line, *l;
  int i, last, first;

  if (!N)
    return;

  line = malloc(N * sizeof(Line *));

  if (!line)
  {
    fprintf(stderr, "Error in tail_reading_all\n");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < N; ++i)
    line[i] = NULL;

  last = 0;

  while ((l = io_readline(stream)))
  {
    if (line[last])
      line_free(line[last]); /* we could just reuse old buffers */

    line[last] = l;
    last = (last + 1) % N;
  }

  last = (N + last - 1) % N;

  first = (last + 1) % N;

  if (!line[first])
    first = 0;

  for (i = first; i != last; i = (i + 1) % N)
  {
    fwrite(line[i]->buf, 1, line[i]->size, stdout);
    fprintf(stdout, "\n");
  }

  if (N && line[last])
  {
    fwrite(line[last]->buf, 1, line[last]->size, stdout);
    fprintf(stdout, "\n");
  }

  for (i = 0; i < N; i++)
  {
    if (line[i])
      line_free(line[i]);
    else
      break;
  }

  free(line);
}


int
main (int argc, char *argv[])
{
  if (argc == 2)
    tail_reading_all(atoi(argv[1]), stdin);
  else if (argc == 3)
  {
    FILE *f;

    f = fopen(argv[2], "r");

    if (!f)
    {
      fprintf(stderr, "Could not open %s\n", argv[2]);
      exit(EXIT_FAILURE);
    }

    tail_reading_all(atoi(argv[1]), f);

    fclose(f);
  }
  else
  {
      fprintf(stderr, "usage: %s n [file]\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
