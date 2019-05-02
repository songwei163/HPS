//
// Created by s on 19-5-2.
//

/*
 * 测试listen监听函数
 * 在监听队列中，处于ESTABLISTENED状态的连接有backlog+1个
 * 多次连接之，其他连接都处于SYN_RCVD状态，所以，可发现完整的
 * 连接最多有backlog+1个。
 */

#include "myutili.h"

static bool stop = false;
static void handle_term (int sig)
{
  stop = true;
}

int main (int argc, char *argv[])
{
  signal (SIGTERM, handle_term);
  if (argc <= 3)
    {
      printf ("usage: %s ip_address port_number backlog\n", basename (argv[0]));
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);
  int backlog = atoi (argv[3]);

  int listenfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_address;
  memset (&serv_address, 0, sizeof (serv_address));
  serv_address.sin_family = AF_INET;
  serv_address.sin_port = port;
  inet_pton (AF_INET, ip, &serv_address.sin_addr);

  Bind (listenfd, (struct sockaddr *) &serv_address, sizeof (serv_address));
  Listen (listenfd, backlog);

  while (!stop)
    {
      sleep (1);
    }
  close (listenfd);
  return 0;
}


