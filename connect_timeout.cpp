//
// Created by s on 19-5-13.
//

/*
 * 设置connect超时时间
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
  int sockfd = timeout_connect (ip, port, 10);
  if (sockfd < 0)
    {
      return 1;
    }
  return 0;
}