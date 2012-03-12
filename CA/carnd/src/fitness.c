#include <fitness.h>
#include <caiterate.h>
#include <util.h>

#include <stdlib.h>

#define HASH_SIZE 1104707
#define INIT_RANDOM 1

static CA *ca1 = 0;
static CA *ca2 = 0;
static CA *ca3 = 0;

struct hash_entry
{
  unsigned int key;
  float value;
};

struct hash_entry hash[HASH_SIZE];

float fitness(unsigned int rule)
{
  long double min_ent, aux;
  const int H = 4.0;

  if (hash[rule % HASH_SIZE].key == rule)
  {
    return hash[rule % HASH_SIZE].value;
  }

  if (!ca1)
  {
    ca1 = ca_create(256, rule);
    ca2 = ca_create(257, rule);
    ca3 = ca_create(255, rule);
  }
  else
  {
    ca_rule_set(ca1, rule);
    ca_rule_set(ca2, rule);
    ca_rule_set(ca3, rule);
  }

  if (INIT_RANDOM)
  {
    ca_init_random(ca1);
    ca_init_random(ca2);
    ca_init_random(ca3);
  }
  else
  {
    ca_init_single(ca1);
    ca_init_single(ca2);
    ca_init_single(ca3);
    ca_iterate(ca1, 32);
    ca_iterate(ca2, 32);
    ca_iterate(ca3, 32);
  }

  min_ent = ca_entropy_measure(ca1, 1024, H);

  aux = ca_entropy_measure(ca2, 1024, H);

  if (aux < min_ent)
    min_ent = aux;

  aux = ca_entropy_measure(ca3, 1024, H);

  if (aux < min_ent)
    min_ent = aux;

  hash[rule % HASH_SIZE].key   = rule;
  hash[rule % HASH_SIZE].value = min_ent;

  return min_ent;
}
