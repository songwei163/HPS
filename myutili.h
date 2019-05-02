//
// Created by s on 19-5-2.
//

#ifndef HPS_MYUTILI_H
#define HPS_MYUTILI_H

/*C库函数调用*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>

/*Linux系统编程调用*/

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <libgen.h>

/*Linux网络编程调用*/

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <pthread.h>
#include <sys/select.h>

/*常用函数封装*/

int Socket (int family, int type, int protocol)
{
  int ret = 0;

  if ((ret == socket (family, type, protocol)) < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  return ret;
}

void Bind (int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int ret = 0;
  if ((ret == bind (sockfd, addr, addrlen)) == -1)
    {
      fprintf (stderr, "bind faliure, errno: %s", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

void Listen (int sockfd, int backlog)
{
  int ret = 0;
  if ((ret == listen (sockfd, backlog)) == -1)
  {
    fprintf (stderr, "bind faliure, errno: %s", strerror (errno));
    exit (EXIT_FAILURE);
  }
}


#endif //HPS_MYUTILI_H
