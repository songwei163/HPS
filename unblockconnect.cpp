//
// Created by s on 19-5-4.
//

#include "myutili.h"

int unblock_connect (const char *ip, int port, int time)
{
  int ret = 0;
  struct sockaddr_in address;
  bzero (&address, sizeof (address));
  address.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &address.sin_addr);
  address.sin_port = htons (port);

  int sockfd = socket (PF_INET, SOCK_STREAM, 0);
  int fdopt = setnonblocking (sockfd);
  ret = connect (sockfd, (struct sockaddr *) &address, sizeof (address));
  if (ret == 0)
    {
      printf ("connect with server immediately\n");
      fcntl (sockfd, F_SETFL, fdopt);
      return sockfd;
    }
  else if (errno != EINPROGRESS)
    {
      printf ("unblock connect not support\n");
      exit (EXIT_FAILURE);
    }

  fd_set readfds;
  fd_set writefds;
  struct timeval timeout;

  FD_ZERO(&readfds);
  FD_SET(sockfd, &writefds);

  timeout.tv_sec = time;
  timeout.tv_usec = 0;

  ret = select (sockfd + 1, NULL, &writefds, NULL, &timeout);
  if (ret <= 0)
    {
      printf ("connection time out\n");
      close (sockfd);
      exit (EXIT_FAILURE);
    }

  if (!FD_ISSET(sockfd, &writefds))
    {
      printf ("no events on sockfd found\n");
      close (sockfd);
      exit (EXIT_FAILURE);
    }

  int error = 0;
  socklen_t length = sizeof (error);
  if (getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &length) < 0)
    {
      printf ("get socket option failed\n");
      close (sockfd);
      exit (EXIT_FAILURE);
    }

  if (error != 0)
    {
      printf ("connection failed after select with the error: %d \n", error);
      close (sockfd);
      exit (EXIT_FAILURE);
    }

  printf ("connection ready after select with the socket: %d \n", sockfd);
  fcntl (sockfd, F_SETFL, fdopt);
  return sockfd;
}

int main (int argc, char *argv[])
{
  if (argc <= 2)
    {
      printf ("usage: %s ip_address port_number\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);

  int sockfd = unblock_connect (ip, port, 10);
  if (sockfd < 0)
    {
      return 1;
    }
  shutdown (sockfd, SHUT_WR);
  sleep (200);
  printf ("send data out\n");
  send (sockfd, "abc", 3, 0);
  //sleep( 600 );
  return 0;
}