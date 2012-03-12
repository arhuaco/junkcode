#include <stdio.h>
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

#include <serial_helper.h>


#define FLAG_ECHO 4
#define FLAG_SNIFF 8 /* send traffic there */
#define FLAG_SIDE_A 16
#define FLAG_SIDE_B 32
#define FLAG_TCP_SERVER 64
#define FLAG_SHOW_IN_LIST 128

int
other_side (int flags)
{
  if (flags & FLAG_SIDE_A)
    return FLAG_SIDE_B;

  if (flags & FLAG_SIDE_B)
    return FLAG_SIDE_A;

  assert(0);
}


/* End points */

typedef struct _EndPoint
{
  char *name;
  char free_name;
  int fd;
  struct _EndPoint *next;
  int flags;
  int son_flags; /* inherited flags for fds returned by connect(2) */
}EndPoint;

EndPoint *
endpoint_new()
{
  EndPoint *n;

  n = malloc(sizeof(EndPoint));
  memset(n, '\0', sizeof(EndPoint));

  if (!n)
  {
    fprintf(stderr, "Out of memory\n");
    exit(1);
  }

  return n;
}

void
endpoint_delete (EndPoint *e)
{
  close(e->fd);
  if (e->free_name)
    free(e->name);
  free(e);
}

EndPoint *
echo_endpoint()
{
  EndPoint *n;

  n = endpoint_new();
  n->fd = -1; /* invalid */

  return n;
}

EndPoint *
udp_endpoint(char *local_host, int lport, char *remote_host, int rport)
{
  struct sockaddr_in addr;
  struct hostent *he;

  EndPoint *n;

  n = endpoint_new();

  if ((n->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  if ((he = gethostbyname(local_host)) == NULL)
  {
    herror("gethostbyname");
    exit(1);
  }


  addr.sin_family = AF_INET;
  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

  /* local part of the connection */
  addr.sin_port = htons(lport);
  addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  if (bind(n->fd, (struct sockaddr *) &addr, sizeof addr) == -1) {
    perror("bind");
    exit(1);
  }

  /* remote part of the connection */

  if ((he = gethostbyname(remote_host)) == NULL)
  {
    herror("gethostbyname");
    exit(1);
  }

  addr.sin_port = htons(rport);
  addr.sin_addr = *((struct in_addr *)he->h_addr); /* not INADDR_ANY ! */

  if (connect(n->fd, (struct sockaddr *)&addr, sizeof addr) == -1)
  {
    perror("connect");
    exit(1);
  }

  return n;
}

EndPoint *
tty_endpoint(char *path, int speed)
{
  EndPoint *n;

  n = endpoint_new();

  n->fd = serial_open(path, speed, 1 /* verbose */ );

  if (n->fd < 0)
  {
    fprintf(stderr, "Couln't open tty enpoint %s\n", path);
    exit(1);
  }

  return n;
}

#if 0

/* I could not make this work. I also used open(2). */

EndPoint *
file_endpoint(char *path)
{
  EndPoint *n;
  FILE *f;

  n = endpoint_new();

  f = fopen(path, "w+");

  if (!f)
  {
    fprintf(stderr, "Couln't open file enpoint %s\n", path);
    exit(1);
  }

  n->fd = fileno(f);

  return n;
}
#endif

EndPoint *
tcp_client (char *remote_host, int port)
{
  int fd;
  struct sockaddr_in their_addr;
  struct hostent *he;
  EndPoint *n;

  if ((he = gethostbyname(remote_host)) == NULL)
  {
    herror("gethostbyname");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(port);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

  if (connect(fd, (struct sockaddr *)&their_addr, sizeof their_addr) == -1)
  {
    perror("connect");
    exit(1);
  }

  n = endpoint_new();
  n->fd = fd;

  return n;
}


EndPoint *
tcp_server (int port, int backlog)
{
  int fd;
  struct sockaddr_in my_addr;
  int yes=1;
  EndPoint *n;


  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("setsockopt");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

  if (bind(fd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1)
  {
    perror("bind");
    exit(1);
  }

  if (listen(fd, backlog) == -1) {
      perror("listen");
      exit(1);
  }

  n = endpoint_new();
  n->fd = fd;

  return n;
}


/* naive list functions */

void
add_to_list(EndPoint **list, EndPoint *new_node)
{
  new_node->next = *list;
  *list = new_node;
}

EndPoint *
search_list_n(EndPoint *list, int N, int flag) /* first element => N = 1 */
{
  EndPoint *l = list, *f;
  int i = 0;

  for (i = 0; l && i < N; l = l->next)
    if (l->flags & flag)
    {
      i++;
      f = l;
    }

  if (i == N) /* found */
    return f;

  return NULL;
}

void
clear_list_flags(EndPoint *list, int flags)
{
  for ( ; list ; list = list->next)
    list->flags &= ~flags;
}

void
remove_from_list(EndPoint **list, EndPoint *del_node)
{
  if (del_node == *list) /* first element ? */
  {
    *list = del_node->next;
  }
  else
  {
    EndPoint *l = *list;

    while (l && l->next != del_node)
      l = l->next;

    assert(l && del_node == l->next); /* the element must be found */
    l->next = l->next->next;
  }

  endpoint_delete(del_node);
}

/* dump data */
void
dump_packet(int fd, char *buf, int len)
{
  char out[16];

  for(; len > 0; --len)
  {
    sprintf(out, " %x", (unsigned char) (*(buf++)));
    write(fd, out, strlen(out));
  }

  write(fd, " --\n", 4);
}

int
write_len(int fd, char *s)
{
  return write(fd, s, strlen(s));
}

#define CONTROL_PORT 5001
#define SNIFF_PORT 5002
#define BRIDGE_PORT 5003
#define BUF_SIZE 512

int
main(int argc, char *argv[])
{
  EndPoint *list = NULL, *n, *tmp;
  fd_set read_set;
  char buf[BUF_SIZE];
  struct sockaddr_in client_addr;
  socklen_t sin_size = sizeof client_addr;

  n = echo_endpoint();
  add_to_list(&list, n);
  n->name = "ECHO";
  n->flags = FLAG_SIDE_A | FLAG_SHOW_IN_LIST | FLAG_ECHO;

  n = tty_endpoint("/dev/ttyS1", 57600);
  add_to_list(&list, n);
  n->name = "RS485 (ttyS1)";
  n->flags = FLAG_SIDE_B | FLAG_SHOW_IN_LIST;

#if 0
  n = tcp_client("192.168.1.197", BRIDGE_PORT); /* A bridge client */
  add_to_list(&list, n);
  n->name = "GAIRA NITMS";
  n->flags = FLAG_SHOW_IN_LIST  | FLAG_SIDE_B;

  n = tcp_server(BRIDGE_PORT, 5); /* A bridge server ! */
  add_to_list(&list, n);
  n->name = "TCP Server";
  n->flags = FLAG_TCP_SERVER ;
  n->son_flags = FLAG_SHOW_IN_LIST;
#endif

# if 0
  n = udp_endpoint("localhost", 2000, "localhost", 2001);
  add_to_list(&list, n);
  n->name = "QEMU";
  n->flags = FLAG_SIDE_A | FLAG_SHOW_IN_LIST;

  n = udp_endpoint("localhost", 2002, "localhost", 2003);
  add_to_list(&list, n);
  n->name = "UDPVM";
  n->flags = FLAG_SHOW_IN_LIST;

  n = udp_endpoint("localhost", 2004, "localhost", 2005);
  add_to_list(&list, n);
  n->name = "DebugClient";
  n->flags = FLAG_SHOW_IN_LIST;

  n = tty_endpoint("/dev/ttyUSB0", 9600);
  add_to_list(&list, n);
  n->name = "CKUSB";
  n->flags = FLAG_SIDE_B | FLAG_SHOW_IN_LIST;

  n = tty_endpoint("/dev/ttyS0", 9600);
  add_to_list(&list, n);
  n->name = "CKTTY";
  n->flags = FLAG_SHOW_IN_LIST;

  n = tty_endpoint("/dev/ttyUSB1", 38400);
  add_to_list(&list, n);
  n->name = "SAM7ECOS";
  n->flags = FLAG_SHOW_IN_LIST;
#endif

  n = tcp_server(SNIFF_PORT, 5);
  add_to_list(&list, n);
  n->name = "TCP Server";
  n->flags = FLAG_TCP_SERVER ;
  n->son_flags = FLAG_SNIFF;

  fprintf(stderr, "Please connect to port %d to sniff traffic\n", SNIFF_PORT);

  for  (tmp = list; tmp; tmp = tmp->next)
  {
    if ( !(tmp->flags & FLAG_ECHO) && fcntl(tmp->fd, F_SETFD, O_NONBLOCK) == -1)
    {
      perror("fcntl");
      exit(1);
    }
  }

  while (1)
  {
    EndPoint *who = NULL;
    struct timeval tv;
    int max;
    int fd;

    FD_ZERO(&read_set);

    max = 0;
    for  (tmp = list; tmp; tmp = tmp->next)
      if (!(tmp->flags & FLAG_ECHO) && tmp->flags)
      {
        FD_SET(tmp->fd, &read_set);
        if (tmp->fd > max)
          max = tmp->fd;
      }

    tv.tv_sec = 30;
    tv.tv_usec = 0;

    fprintf(stderr, "select\n");

    if ((fd = select(max + 1, &read_set, NULL, NULL, &tv)) == -1)
    {
      perror("select");
      exit(1);
    }

    fprintf(stderr, "select ended\n");

    for (who = list; fd && who ; who = who->next)
      if (!(who->flags & FLAG_ECHO) && FD_ISSET(who->fd, &read_set))
      {

        if (who->flags & (FLAG_SIDE_A|FLAG_SIDE_B)) /* must pass the traffic */
        {
          EndPoint *to;
          int other_flag = other_side(who->flags);
          int r, w;

          for (to = list; to; to = to->next)
            if (to->flags & other_flag)
             break;

          if (to->flags & FLAG_ECHO)
            to = who;

          if (!to)
          {
            fprintf(stderr, "Data available from %s and no destination available\n", who->name);
            /* We might want to do something better here  */
            exit(1);
          }

          if ((r = read(who->fd, buf, BUF_SIZE)) == -1)
          {
            perror("read");
            exit(1);
          }

          if (!r)
          {
            fprintf(stderr, "endpoint %s closed\n", who->name);
            /* one side closed, close the other one */
            clear_list_flags(list, FLAG_SIDE_A | FLAG_SIDE_B);
            remove_from_list(&list, who);
          }

          if (r && (w = write(to->fd, buf, r)) == -1) /* TODO: FIX, we might get SIGPIPE here */
          {
            perror("write");
            exit(1);
          }

          if (r && r != w)
          {
            fprintf(stderr, "Not handling partial writes yet\n");
            exit(1);
          }

          /* dump the packet */
          for (tmp = list; r && tmp; tmp = tmp->next)
            if (tmp->flags & FLAG_SNIFF)
            {
              char lbuf[32];
              int l;

             /* TODO: get this working 
              float secs;
              struct timespec t;
              clock_gettime (CLOCK_REALTIME, &t);
              secs = (float)(t.tv_sec) + (float)(t.tv_nsec*(1e-9));
              l = snprintf(lbuf, 32, " (time:%.06f) ", secs);
              assert( l <= 31 && l >= 0);
              write(tmp->fd, lbuf, l);
              */

              write (tmp->fd, who->name, strlen(who->name));
              write (tmp->fd, " -> ", 4);
              write (tmp->fd, to->name, strlen(to->name));
              l = snprintf(lbuf, 32, " (len:%d) hex:", w);
              assert( l <= 31 && l >= 0);
              write(tmp->fd, lbuf, l);
              dump_packet(tmp->fd, buf, w);
            }
        } /* (SIDE_A or  SIDE_B) */
        else if (who->flags & FLAG_TCP_SERVER) /* accept connection */
        {
          int fd;

          if ((fd = accept(who->fd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
             perror("accept");
          else
          {
            n = endpoint_new();
            n->fd = fd;
            n->free_name = 1;
            n->name = strdup(inet_ntoa(client_addr.sin_addr));
            n->flags = who->son_flags;
            add_to_list(&list, n);

            fprintf(stderr, "Got connection from %s\n", n->name);

            if (fcntl(n->fd, F_SETFD, O_NONBLOCK) == -1)
            {
              perror("fcntl");
              exit(1);
            }

            if (n->flags & FLAG_SNIFF)
              write_len(n->fd, "\n\nWelcome to nitm\ntype 'help' if you don't know what to do.\n\n");
          }
        }
        else if (who->flags & FLAG_SNIFF) /* must pass the traffic */
        {
          int r;

          if ((r = read(who->fd, buf, BUF_SIZE - 1)) == -1)
          {
            perror("read");
            exit(1);
          }

          if (!r)
          {
            fprintf(stderr, "client %s closed\n", who->name);

            remove_from_list(&list, who);
          }
          else
          {
            buf[r] = '\0';
            char *c = buf;
            int i;

            /* command. Let's trim. */
            while (isspace(*c))
              c++;
            for (i = strlen(c) - 1; i >= 0 && isspace(c[i]); --i)
              c[i] = '\0';

            if (!*c); /* user making space */
            else if (!strcasecmp(c, "quit"))
            {
              fprintf(stderr, "client %s quit\n", who->name);
              remove_from_list(&list, who);
            }
            else if (!strcasecmp(c, "list"))
            {
              char lbuf[32];
              int count = 1;
              for (tmp = list; tmp; tmp = tmp->next)
                if (tmp->flags & FLAG_SHOW_IN_LIST)
                {
                  snprintf(lbuf, 31, "%d => %c ", count++,
                    tmp->flags & (FLAG_SIDE_A | FLAG_SIDE_B) ? '*' : ' ');
                  write_len(who->fd, lbuf);
                  write_len(who->fd, tmp->name);
                  write_len(who->fd, "\n");
                }
            }
            else if (!strncasecmp(c, "use", 3))
            {
              int i, j;
              int error = 0;
              char *cj;

              /* look for 2 numbers in the rest of the command */

              i = strtol(c + 3, &cj, 10);
              if (cj)
              {
                while (isspace(*cj))
                  cj++;
                if (isdigit(*cj))
                {
                  j = strtol(cj, &cj, 10);
                  if (*cj)
                    error = 1;
                }
                else
                  error = 1;
              }
              else
                error = 1;

              if (!error && i > 0 && j > 0)
              {
                EndPoint *oa = search_list_n(list, i, FLAG_SHOW_IN_LIST);
                EndPoint *ob = search_list_n(list, j, FLAG_SHOW_IN_LIST);

                if (oa && ob)
                {
                  for (tmp = list; tmp; tmp = tmp->next)
                    tmp->flags &= ~(FLAG_SIDE_A | FLAG_SIDE_B);
                    oa->flags |= FLAG_SIDE_A;
                    ob->flags |= FLAG_SIDE_B;
                    write_len(who->fd, "Selected : ");
                    write_len(who->fd, oa->name);
                    write_len(who->fd, " and ");
                    write_len(who->fd, ob->name);
                    write_len(who->fd, "\n");
                }
                else
                  write_len(who->fd, "Error. Index not found. Run 'list'\n");
              }
              else
                write_len(who->fd, "Error. 'use' needs two numbers. Ie: 'use 1 2'. Run 'list'\n");
            }
            else /* help */
            {
              char *h = "\nlist -- list endpoints\n"
                        "use x y  -- Connect x and y (use 'list' first)\n"
                        "quit  -- disconnnect from the server\n\n";

              write_len(who->fd, h);
            }

          } /* a command */
        }
        else
        /* Discard traffic, not in a current connection. You might want to use
         * a buffer here. Do you need a buffer here? */
        {
          int r;

          if ((r = read(who->fd, buf, BUF_SIZE)) == -1)
          {
            perror("read");
            exit(1);
          }

          if (!r)
          {
            fprintf(stderr, "Endpoint %s closed\n", who->name);

            remove_from_list(&list, who);
          }
          else
            fprintf (stderr, "Discarded %d bytes from '%s'\n", r, who->name);
        }
      }
    }

  return 0;
}
