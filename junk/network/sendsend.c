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

/* netcat -u is dying when the remote UDP port is not listening */

#define LEN 512

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in their_addr;
    struct hostent *he;
    int numbytes;
    fd_set rfds;
    struct timeval tv;
    int retval;
    char buf[LEN];

    if (argc != 3) {
        fprintf(stderr,"options:  hostname port\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(atoi(argv[2]));
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    while (1)
    {
      FD_ZERO(&rfds);
      FD_SET(0, &rfds);

      tv.tv_sec = 0;
      tv.tv_usec = 100000; /* 100 ms */

      retval = select(1, &rfds, NULL, NULL, &tv);

      if (retval == -1)
      {
        if (errno != EINTR)
        {
          perror("select");
          exit(1);
        }
      }
      else if (retval && FD_ISSET(0, &rfds))
      {
        numbytes = read(0, buf, LEN);

        if (!numbytes)
          exit(0);

        if ((numbytes = sendto(sockfd, buf, numbytes, 0,
             (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
                perror("sendto");
                exit(1);
        }
      }
    }
}
