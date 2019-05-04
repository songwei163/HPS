//
// Created by s on 19-5-4.
//

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 2)
    {
      printf ("usage: %s ip_address port_number\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);

  int sockfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  Connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  pollfd fds[2];
  fds[0].fd = 0;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  fds[1].fd = sockfd;
  fds[1].events = POLLIN | POLLRDHUP;
  fds[1].revents = 0;
  char read_buf[BUFFER_SIZE];
  int pipefd[2];
  int ret = pipe (pipefd);
  assert(ret != -1);

  while (1)
    {
      ret = poll (fds, 2, -1);
      if (ret < 0)
        {
          printf ("poll failure\n");
          break;
        }

      if (fds[1].revents & POLLRDHUP)
        {
          printf ("server close the connection\n");
          break;
        }
      else if (fds[1].revents & POLLIN)
        {
          memset (read_buf, '\0', BUFFER_SIZE);
          recv (fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
          printf ("%s\n", read_buf);
        }

      if (fds[0].revents & POLLIN)
        {
          ret = splice (0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
          ret = splice (pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
    }

  close (sockfd);
  return 0;
}