#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>

/* http://www.webservertalk.com/message891078.html
 *
 * How to read the MAC address from C.
 *
 */

int
main(void)
{
    int sfd;
    unsigned char *u;
    struct ifreq ifr;
    struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;

    memset(&ifr, 0, sizeof ifr);

    if (0 > (sfd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    strcpy(ifr.ifr_name, "eth0");
    sin->sin_family = AF_INET;

    /* this is for the IP */
    if (0 == ioctl(sfd, SIOCGIFADDR, &ifr)) {
       printf("%s: %s\n", ifr.ifr_name, inet_ntoa(sin->sin_addr));
    }

    if (0 > ioctl(sfd, SIOCGIFHWADDR, &ifr)) {
        return EXIT_FAILURE;
    }

    u = (unsigned char *) &ifr.ifr_addr.sa_data;

    if (u[0] + u[1] + u[2] + u[3] + u[4] + u[5]) {
        printf("HW Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
               u[0], u[1], u[2], u[3], u[4], u[5]);
    }

    return EXIT_SUCCESS;
}
