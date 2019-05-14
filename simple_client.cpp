//
// Created by s on 19-5-14.
//

/*
 * 一个简单的客户端例子
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

  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons (port);
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);

  int sockfd = Socket (AF_INET, SOCK_STREAM, 0);
  Connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  const char *test_data = "send test data\n";
  while (1)
    {
      send (sockfd, test_data, strlen (test_data) + 1, 0);
    }

  close (sockfd);
  return 0;
}