//
// Created by s on 19-5-2.
//

/*
 * 测试accep函数
 * 说明accept只是从监听队列中取出连接，而不关心连接状态，
 * 更不关心任何网络状态的变化
 */

#include "myutili.h"

#define LISTEN_QUEUE 5

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
          inet_ntop (AF_INET, &cli_addr.sin_addr, remote, INET_ADDRSTRLEN));

  close (connfd);
  close (listenfd);
  return 0;
}

