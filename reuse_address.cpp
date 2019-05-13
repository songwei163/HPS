//
// Created by s on 19-5-2.
//

/*
 * 重用本地地址
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

  int listenfd = Socket (AF_INET, SOCK_STREAM, 0);
  int reuse = 1;
  Setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse));

  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  Bind (listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  Listen (listenfd, LISTEN_QUEUE);

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  char remote[INET_ADDRSTRLEN];
  printf ("connected with ip: %s and port: %d\n",
          inet_ntop (AF_INET, &cli_addr.sin_addr, remote, INET_ADDRSTRLEN), ntohs (cli_addr.sin_port));

  close (connfd);
  close (listenfd);
  return 0;
}