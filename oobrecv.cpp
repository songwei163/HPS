//
// Created by s on 19-5-2.
//

/*
 * 接收oob带外数据
 * 服务器对正常数据的接收将被带外数据截断
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

  int ret = 0;
  char buff[BUF_SIZE];
  memset (buff, '\0', sizeof (buff));
  ret = Recv (connfd, buff, BUF_SIZE - 1, 0);
  printf ("got %d bytes of normal data '%s'\n", ret, buff);

  memset (buff, '\0', sizeof (buff));
  ret = Recv (connfd, buff, BUF_SIZE - 1, MSG_OOB);
  printf ("got %d bytes of oob data '%s'\n", ret, buff);

  memset (buff, '\0', sizeof (buff));
  ret = Recv (connfd, buff, BUF_SIZE - 1, 0);
  printf ("got %d bytes of normal data '%s'\n", ret, buff);

  close (connfd);
  close (listenfd);
  return 0;
}
