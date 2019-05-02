//
// Created by s on 19-5-2.
//

/*
 * 设置TCP接收缓冲区大小
 * 最小值为256 bytes
 * 可设置内核参数取消最小值限制
 */

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 3)
    {
      printf ("usage: %s ip_address port_number recv_buf_size\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);
  int recvbuf = atoi (argv[3]);

  int listenfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  int len = sizeof (recvbuf);
  setsockopt (listenfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof (recvbuf));
  getsockopt (listenfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, (socklen_t *) &len);
  printf ("the receive buffer size after settting is %d\n", recvbuf);

  Bind (listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  Listen (listenfd, LISTEN_QUEUE);

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  char buffer[BUFFER_SIZE];
  memset (buffer, '\0', BUFFER_SIZE);
  while (Recv (connfd, buffer, BUFFER_SIZE - 1, 0) > 0);

  close (connfd);
  close (listenfd);
  return 0;
}
