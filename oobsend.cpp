//
// Created by s on 19-5-2.
//

/*
 * 发送oob带外数据
 * oob_data abc
 * normal_data 123
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

  int sockfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  Connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  printf ("send oob data out\n");
  const char *oob_data = "abc";
  const char *normal_data = "123";
  send (sockfd, normal_data, strlen (normal_data), 0);
  send (sockfd, oob_data, strlen (oob_data), MSG_OOB);
  send (sockfd, normal_data, strlen (normal_data), 0);

  close (sockfd);
  return 0;
}

