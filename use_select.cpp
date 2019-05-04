//
// Created by s on 19-5-4.
//

/*
 * use_select
 */

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

  int listenfd = Start_TCPserver_up (ip, port);

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  char remote_addr[INET_ADDRSTRLEN];
  printf ("connected with ip: %s and port: %d\n",
          inet_ntop (AF_INET, &cli_addr.sin_addr, remote_addr, INET_ADDRSTRLEN), ntohs (cli_addr.sin_port));

  char buf[1024];
  fd_set read_fds;
  fd_set exception_fds;

  FD_ZERO(&read_fds);
  FD_ZERO(&exception_fds);

  int nReuseAddr = 1;
  setsockopt (connfd, SOL_SOCKET, SO_OOBINLINE, &nReuseAddr, sizeof (nReuseAddr));
  int ret = 0;

  while (1)
    {
      memset (buf, '\0', sizeof (buf));
      FD_SET(connfd, &read_fds);
      FD_SET(connfd, &exception_fds);

      ret = select (connfd + 1, &read_fds, NULL, &exception_fds, NULL);
      printf ("select one\n");
      if (ret < 0)
        {
          printf ("selection failure\n");
          break;
        }

      if (FD_ISSET(connfd, &read_fds))
        {
          ret = recv (connfd, buf, sizeof (buf) - 1, 0);
          if (ret <= 0)
            {
              break;
            }
          printf ("get %d bytes of normal data: %s\n", ret, buf);
        }
      else if (FD_ISSET(connfd, &exception_fds))
        {
          ret = recv (connfd, buf, sizeof (buf) - 1, MSG_OOB);
          if (ret <= 0)
            {
              break;
            }
          printf ("get %d bytes of oob data: %s\n", ret, buf);
        }

    }

  close (connfd);
  close (listenfd);
  return 0;
}