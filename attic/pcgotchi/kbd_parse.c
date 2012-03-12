static void kst_sq_set (unsigned char start , char* seq, unsigned char *v)
{
  int i;

  for (i = start; *seq; ++i)
    v[i] = *(seq++);
}

unsigned int         kst_count[256];
static unsigned char kst_map[256];

static void kbd_parse (unsigned char sc)
{
   int release;
   unsigned char key;

   release = 0x80 & sc;
   key = (~0x80) & sc;

   kst_sq_set(16, "qwertyuiop", map);
   kst_sq_set(30, "asdfghjklñ", map);
   kst_sq_set(44, "zxcvbnm", map);

   if (!release && map[key])
      kst_count[key] ++;
}
