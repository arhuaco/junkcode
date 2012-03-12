#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

struct sockaddr_in local_addr;

int
udp_local(char *local_host, int lport)
{
  struct sockaddr_in addr;
  struct hostent *he;
  int fd;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)  {
    perror("socket");
    exit(1);
  }

  if ((he = gethostbyname(local_host)) == NULL) {
    herror("gethostbyname");
    exit(1);
  }

  addr.sin_family = AF_INET;
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  /* local part of the connection */
  addr.sin_port = htons(lport);
  addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  if (bind(fd, (struct sockaddr *) &addr, sizeof addr) == -1) {
    perror("bind");
    exit(1);
  }

  return fd;
}

int
udp_remote(char *local_host, int lport, char *remote_host, int rport)
{
  struct sockaddr_in addr;
  struct hostent *he;
  int fd;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)  {
    perror("socket");
    exit(1);
  }

  if ((he = gethostbyname(local_host)) == NULL) {
    herror("gethostbyname");
    exit(1);
  }

  addr.sin_family = AF_INET;
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  /* local part of the connection */
  addr.sin_port = htons(lport);
  addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  if (bind(fd, (struct sockaddr *) &addr, sizeof addr) == -1) {
    perror("bind");
    exit(1);
  }

  if ((he = gethostbyname(remote_host)) == NULL) {
    herror("gethostbyname");
    exit(1);
  }

  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
  addr.sin_port = htons(rport);
  addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  if (connect(fd, (struct sockaddr *)&addr, sizeof addr) == -1) {
    perror("connect");
    exit(1);
  }

  return fd;
}


int fd, fdr;

void *echo(void *threadid)
{
   int src;
   int dst;
   char buf[65535];
   if ((long)threadid)
     src = fd, dst = fdr;
   else
     src = fdr, dst = fd;

  while (1) {
    int r, w;
    if ((r = read(src, buf, sizeof(buf))) == -1) {
      perror("read");
      exit(1);
    }
    if (dst == fd) {
      w = sendto(dst, buf, r, 0, (struct sockaddr *)&local_addr,sizeof(local_addr));
    }
    else
      w = write(dst, buf, r);
    fprintf(stderr, "%ld r:%d w:%d\n", (long)threadid, r, w);
  }
}

#define AVAYA_PROXY_IP "127.0.0.1"

#define PROTO_PORT 9999

#define INTERNAL_IP "127.0.0.1"
#define EXTERNAL_IP "192.168.1.108"

#define REMOTE_IP "74.125.47.105"


int
main(int argc, char *argv[])
{
  pthread_t threads[2];
  long t;

  /* para sendto */
  struct hostent *he;
  if ((he = gethostbyname(AVAYA_PROXY_IP)) == NULL) { // el otro proxy
    herror("gethostbyname");
    exit(1);
  }
  local_addr.sin_family = AF_INET;
  memset(local_addr.sin_zero, '\0', sizeof local_addr.sin_zero);
  /* local part of the connection */
  local_addr.sin_port = htons(PROTO_PORT); // el otro proxy
  local_addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  fd = udp_local(INTERNAL_IP, PROTO_PORT); // donde este proxy recibe
  fdr = udp_remote(EXTERNAL_IP, PROTO_PORT, REMOTE_IP, PROTO_PORT); // el que tiene problemas

  for(t = 0; t < 2; t++) {
     int rc = pthread_create(&threads[t], NULL, echo, (void *)t);
     assert(!rc);
   }

   pthread_join(*threads, NULL); /* nunca */

  return 0;
}
