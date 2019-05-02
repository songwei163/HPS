//
// Created by s on 19-5-2.
//

/*
 * 设置TCP发送缓冲区大小
 * 最小值为2048 bytes
 * 为处理拥塞
 * 可设置内核参数取消最小值限制
 */

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 3)
    {
      printf ("usage: %s ip_address port_number send_buf_size\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);
  int sendbuf = atoi (argv[3]);

  int sockfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  int len = sizeof (sendbuf);
  setsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof (sendbuf));
  getsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, (socklen_t *) &len);

  printf ("the tcp send buffer size after setting is %d\n", sendbuf);

  Connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  char buffer[BUFFER_SIZE];
  memset (buffer, 'a', BUFFER_SIZE);
  Send (sockfd, buffer, BUFFER_SIZE, 0);

  close (sockfd);
  return 0;
}


