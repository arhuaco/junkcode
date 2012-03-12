#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include <assert.h>

// Enables assert
#define DEBUG

#define uint32 unsigned int
#define uint16 unsigned short int
#define uint8 unsigned char

// TODO: implement
uint32
uint32_to_le(uint32 x)
{
  if (__BYTE_ORDER != __LITTLE_ENDIAN)
     return (((x&0x000000FF)<<24)+((x&0x0000FF00)<<8)+
        ((x&0x00FF0000)>>8)+((x&0xFF000000)>>24));
  return x;
}

uint16
uint16_to_le(uint16 x)
{
  if(__BYTE_ORDER != __LITTLE_ENDIAN)
     return (((x >> 8)) | (x << 8));
  return x;
}

uint32
uint32_to_be(uint32 x)
{
  if (__BYTE_ORDER == __LITTLE_ENDIAN)
     return (((x&0x000000FF)<<24)+((x&0x0000FF00)<<8)+
        ((x&0x00FF0000)>>8)+((x&0xFF000000)>>24));
  return x;
}

void
print_binary32(FILE *f, uint32 x)
{
  if (fwrite(&x, 4, 1, f) != 1)
  {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }
}

void
print_binary16(FILE *f, uint16 x)
{
  if (fwrite(&x, 2, 1, f) != 1)
  {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }
}

void
riff_chunk(FILE *f, uint32 size)
{
  size = uint32_to_le(size);
  fprintf(f, "RIFF");
  
  print_binary32(f,size);

  fprintf(f, "WAVE");
}

typedef struct 
{ 
  int is_stereo;
  uint32 sample_rate;
  uint32 nbits;
} TWAV_CONF;

void
fmt_chunk(FILE *f, TWAV_CONF conf)
{
  unsigned int bytes_per_sample = (conf.is_stereo ? 0x02 : 0x01) * (conf.nbits / 8);
  unsigned int bits_per_second  = bytes_per_sample * conf.sample_rate;

  assert(bytes_per_sample == 1 || bytes_per_sample == 2 || bytes_per_sample == 4);

  fprintf(f, "fmt ");
  print_binary32(f, uint32_to_le(0x10));
  print_binary16(f, uint16_to_le(0x01));
  print_binary16(f, uint16_to_le(conf.is_stereo ? 0x02 : 0x01));
  print_binary32(f, uint32_to_le(conf.sample_rate));
  print_binary32(f, uint32_to_le(bits_per_second));
  print_binary16(f, uint16_to_le(bytes_per_sample));
  print_binary16(f, uint16_to_le(bytes_per_sample * 8));

}

void
data_chunk(FILE *f, TWAV_CONF conf, uint8 *buff, uint32 nsamples)
{
  uint32 bytes_per_sample = (conf.is_stereo ? 0x02 : 0x01) * (conf.nbits / 8);
  uint32 len = bytes_per_sample * nsamples;

  fprintf(f, "data");

  print_binary32(f, uint32_to_le(len));

  if (fwrite(buff, 1, nsamples, f) != nsamples)
  {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }
}

#include <math.h>

#define SAMPLES 22050

int
main(int argc, char *argv[])
{
  FILE *f = stdout;
  TWAV_CONF conf;
  uint8 buff[SAMPLES];

  float lensine = (float)SAMPLES / 1000.0;

  float i;

  for (i = 0; i < SAMPLES; i += 1.0)
  {
    uint8 v = 128 + 100 * sinf( (i * 2 *  M_PI) / lensine);
    buff[(int)i] = v;
  }

  conf.is_stereo = 0;
  conf.sample_rate = 22050;
  conf.nbits = 8;

  assert(sizeof(uint32) == 4);
  assert(sizeof(uint16) == 2);
  assert(sizeof(uint8) == 1);

  riff_chunk(f, 22086);
  fmt_chunk(f, conf);
  data_chunk(f, conf, buff, SAMPLES);

  return EXIT_SUCCESS;
}
