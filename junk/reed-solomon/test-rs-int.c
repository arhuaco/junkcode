#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fec.h>

void dump_buf(char *filename, int *s, int size)
{
  FILE *p;

  p = fopen(filename, "w");
  if (!p) {
    fprintf(stderr, "Could not write file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }
  fwrite(s, size, sizeof(int), p);
  fclose(p);
}

void test_int()
{
  int nn = 511;
  int roots = 4; // Will recover with up to roots / 2 errors.
  int kk = nn - roots;
  int i;
  int r;
  int nerr = 0;
  int block[nn];
  int blocktmp[nn];
  int pad = 500;
  void *rs;
  int symsize = 9;

  printf("Using RS(%d, %d), it means words of %d %d-bit symbols and %d redundant symbols\n", nn, kk, kk - pad, symsize, roots);

  rs = init_rs_int(symsize, 529, 1, 1, roots, pad);

  for (i = 0; i < kk - pad; ++i)
    block[i] = random() % nn;

  encode_rs_int(rs, block, &block[kk - pad]);

  while (nerr < roots / 2 + 1) {
    nerr++;
    memcpy(blocktmp, block, sizeof(block));
    for (i = 0; i < nerr; ++i)
      while (blocktmp[i] == block[i])
        blocktmp[i] = random() % nn;
    r = decode_rs_int(rs, blocktmp, NULL, 0);

    const char *str = "GOOD";
    if (memcmp(block, blocktmp, sizeof(block)))
      str = "FAIL";

    printf("decode with %d bad symbols returns %d memcmp:%s\n", nerr, r, str);
  };
}

int
main()
{
  test_int();

  return 0;
}
