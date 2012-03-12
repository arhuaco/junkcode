#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <serial_helper.h>

#define SNIFFER_PORT 2000

#define REMOTE_PORT 2001
#define REMOTE_HOSTNAME "localhost"

#define MAXBUFLEN 256
#define MAX(x,y) ((x) > (y) ? (x) : (y))

void
dump_packet(char *desc, unsigned char *buf, int len)
{
  fprintf(stderr, "%s (len:%d) hex:", desc, len);

  while (len--)
    fprintf(stderr, " %x", (unsigned int) (*(buf++)));

  fprintf(stderr, " --\n");
}

int
main(int argc, char *argv[])
{
  int sockfd, serialfd, write_sockfd, fdmax;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr_tmp;
  socklen_t addr_len;
  int numbytes;
  unsigned char buf[MAXBUFLEN];
  fd_set master_set;
  fd_set read_set;

  struct sockaddr_in their_addr;
  struct hostent *he;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s serial-port\n", argv[0]);
    exit(1);
  }

  if ((serialfd = serial_open(argv[1], 9600, 1 /* verbose */)) < 0)
    exit(1);

  if (fcntl(serialfd, F_SETFD, O_NONBLOCK) == -1)
  {
    perror("fcntl");
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(SNIFFER_PORT);
  my_addr.sin_addr.s_addr = INADDR_ANY; /* TODO: Improve. Not secure. */
  memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof my_addr) == -1) {
    perror("bind");
    exit(1);
  }

  /* now the remote host */

  if ((he = gethostbyname(REMOTE_HOSTNAME)) == NULL)
  {
    herror("gethostbyname");
    exit(1);
  }

  if ((write_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(REMOTE_PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

  /* prepare to listen for UDP packets or data from the serial port */

  FD_ZERO(&master_set);
  FD_ZERO(&read_set);

  FD_SET(sockfd, &master_set);
  FD_SET(serialfd, &master_set);
  fdmax = MAX(sockfd, serialfd);

  addr_len = sizeof their_addr_tmp;

  while (1)
  {
    read_set = master_set;

    if (select(fdmax + 1, &read_set, NULL, NULL, NULL) == -1) {
       perror("select");
       exit(1);
    }

    if (FD_ISSET(sockfd, &read_set))
    {
      if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN, 0,
                           (struct sockaddr *) &their_addr_tmp,
                           &addr_len)) == -1)
      {
        perror("recvfrom");
        exit(1);
      }

      if (write(serialfd, buf, numbytes) != numbytes)
      {
        /* Let's not manage partial writes yet */
        fprintf(stderr, "Write to serial port failed\n");
        exit(1);
      }

      dump_packet("udp > tty", buf, numbytes);
    }
    else if (FD_ISSET(serialfd, &read_set))
    {
      int nread;

      if ((nread = read(serialfd, buf, MAXBUFLEN)) == -1)
      {
        perror("sendto");
        exit(1);
      }

      dump_packet("tty > udp", buf, nread);

      if ((numbytes = sendto(write_sockfd, buf, nread, 0,
           (struct sockaddr *)&their_addr, sizeof their_addr)) == -1)
      {
        perror("sendto");
        exit(1);
      }
    }
    else
    {
      fprintf(stderr, " What? Unknown file descriptor\n");
      exit(1);
    }
  }

  /* never reached */
  close(sockfd);
  close(serialfd);
  return 0;
}
