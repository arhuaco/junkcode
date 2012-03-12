#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fec.h>

void dump_buf(char *filename, unsigned char *s, int size)
{
  FILE *p;

  p = fopen(filename, "w");
  if (!p) {
    fprintf(stderr, "Could not write file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }
  fwrite(s, size, 1, p);
  fclose(p);
}

void test_char()
{
  int nn = 255;
  int roots = 8;
  int kk = nn - roots;
  int i;
  int r;
  int nerr = 0;
  unsigned char block[nn];
  unsigned char blocktmp[nn];
  int pad = 215;
  void *rs;

  printf("Using RS(%d, %d), it means words of %d 8-bit symbols and %d redundant symbols\n", nn, kk, kk - pad, roots);

  rs = init_rs_char(8, 0x187, 112, 11, roots, pad);

  memset(block, 0, nn);

  for (i = 0; i < kk - pad; ++i)
    block[i] = 'a' + i % 26;

  dump_buf("dump-before", block, nn);
  encode_rs_char(rs, block, &block[kk - pad]);
  dump_buf("dump-after", block, nn);

  do {
    nerr ++;
    memcpy(blocktmp, block, nn);
    for (i = 0; i < nerr; ++i)
      blocktmp[i] = ~blocktmp[i];
    if (nerr == 16)
      dump_buf("dump-decode-16-before", blocktmp, nn);
    r = decode_rs_char(rs, blocktmp, NULL, 0);
    if (nerr == 16)
      dump_buf("dump-decode-16-after", blocktmp, nn);
    printf("decode with %d bad symbols returns %d\n", nerr, r);
  } while (r >= 0 && nerr < 18);
}

int
main()
{
  test_char();

  return 0;
}
