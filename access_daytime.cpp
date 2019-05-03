//
// Created by s on 19-5-2.
//

/*
 * 通过主机名和服务名来获取目标主机上的daytime服务，
 * 以获取该机器的系统时间
 */

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 2)
    {
      printf ("usage: %s host\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  char *host = argv[1];
  struct hostent *hostinfo = Gethostbyname (host);
  struct servent *servinfo = Getservbyname ("daytime", "tcp");

  printf ("daytime port is %d\n", ntohs (servinfo->s_port));

  int sockfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
  serv_addr.sin_port = servinfo->s_port;

  Connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  char buffer[BUF_SIZE];
  int ret = 0;
  ret = Read (sockfd, buffer, sizeof (buffer));
  buffer[ret] = '\0';
  printf ("the day time is: %s", buffer);
  close (sockfd);
  return 0;
}
