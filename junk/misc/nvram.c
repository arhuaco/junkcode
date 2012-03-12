#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/* this program writes the nvram of a 386 CPU.
 * Note that this is not safe and the bytes we touch might
 * be actually in use in your board */

#define NVRAM_LINUX_BYTE 37 /* /dev/nvram hides the first 14 bytes, so this is actually 37 + 14 */
#define NVRAM_LINUX_SIZE 114 /* again, it is 128 but linux hides 14 bytes */

unsigned char
nvram_read(void)
{
  int f;
  unsigned char buf[NVRAM_LINUX_SIZE];
  unsigned char r = 7;

  if ((f = open("/dev/nvram", O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open /dev/nvram\n");
    return r;
  }
  else if (read(f, buf, NVRAM_LINUX_SIZE) != NVRAM_LINUX_SIZE)
  {
    fprintf(stderr, "could not read /dev/nvram\n");
  }
  else
  {
    r =  buf[NVRAM_LINUX_BYTE];
  }

  close(f);

  return r;
}

unsigned char
nvram_write(unsigned char value)
{
  int f;
  unsigned char buf[NVRAM_LINUX_SIZE];
  unsigned char r = 1;

  if ((f = open("/dev/nvram", O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open /dev/nvram\n");
    return r;
  }
  else if (read(f, buf, NVRAM_LINUX_SIZE) != NVRAM_LINUX_SIZE)
  {
    fprintf(stderr, "could not read /dev/nvram\n");
    close(f);
    return r;
  }

  close(f);

  if ((f = open("/dev/nvram", O_RDWR)) == -1)
  {
    fprintf(stderr, "could not open /dev/nvram\n");
    return r;
  }

  buf[NVRAM_LINUX_BYTE] = (buf[NVRAM_LINUX_BYTE] & 0xf0) | (value & 0x0f);

  if (write(f, buf, NVRAM_LINUX_SIZE) != NVRAM_LINUX_SIZE)
  {
    fprintf(stderr, "could not write /dev/nvram\n");
  }
  else
    r = 0; /* ok */

  close(f);

  return r;
}

int
main(int argc, char *argv[])
{
  fprintf(stderr, "x: 0x%x finishing...\n", nvram_read());
  fprintf(stderr, "x: 0x%x finishing...\n", nvram_read());
  if (nvram_write(9))
    fprintf(stderr, "could not write NVRAM\n");
  else
    fprintf(stderr, "wrote NVRAM\n");

  return 0;
}
