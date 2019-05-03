//
// Created by s on 19-5-2.
//  自定义编写头文件
//

#ifndef HPS_MYUTILI_H
#define HPS_MYUTILI_H

#define LISTEN_QUEUE 5
#define BUF_SIZE 1024
#define BUFFER_SIZE 512


/*C库函数调用*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <csignal>

/*Linux系统编程调用*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/sendfile.h>

//#include <libgen.h>

/*Linux网络编程调用*/

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <pthread.h>
#include <sys/select.h>
#include <netdb.h>

/*常用函数封装*/

int Socket (int family, int type, int protocol)
{
  int ret = 0;

  if ((ret = socket (family, type, protocol)) < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  return ret;
}

void Bind (int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  if (bind (sockfd, addr, addrlen) == -1)
    {
      fprintf (stderr, "bind failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

void Listen (int sockfd, int backlog)
{
  if (listen (sockfd, backlog) == -1)
    {
      fprintf (stderr, "listen failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

int Accept (int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int ret = 0;

  if ((ret = accept (sockfd, addr, addrlen)) == -1)
    {
      fprintf (stderr, "accept failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

void Connect (int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  if (connect (sockfd, addr, addrlen) == -1)
    {
      fprintf (stderr, "connect failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

void Send (int sockfd, const char *buf, size_t len, int flags)
{
  if (send (sockfd, buf, len, flags) == -1)
    {
      fprintf (stderr, "send failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

int Recv (int sockfd, void *buf, size_t len, int flags)
{
  int ret = 0;

  if ((ret = recv (sockfd, buf, len, flags)) == -1)
    {
      fprintf (stderr, "recv failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

struct hostent *Gethostbyname (const char *name)
{
  struct hostent *ret = NULL;

  if ((ret = gethostbyname (name)) == NULL)
    {
      fprintf (stderr, "gethostbyname failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

struct servent *Getservbyname (const char *name, const char *proto)
{
  struct servent *ret = NULL;

  if ((ret = getservbyname (name, proto)) == NULL)
    {
      fprintf (stderr, "getservbyname failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

int Read (int fd, void *buf, size_t count)
{
  int ret = 0;

  if ((ret = read (fd, buf, count)) == -1)
    {
      fprintf (stderr, "read failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

int Dup (int oldfd)
{
  int newfd = 0;

  if ((newfd = dup (oldfd)) == -1)
    {
      fprintf (stderr, "read failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return newfd;
}

int Start_TCPserver_up (const char *ip, int port)
{
  int listenfd = Socket (AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  inet_pton (AF_INET, ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons (port);

  Bind (listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));

  Listen (listenfd, LISTEN_QUEUE);

  return listenfd;
}

void Stat (const char *pathname, struct stat *statbuf)
{
  if (stat (pathname, statbuf) == -1)
    {
      fprintf (stderr, "stat failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }
}

int Open (const char *pathname, int flags)
{
  int ret = 0;

  if ((ret = open (pathname, flags)) == -1)
    {
      fprintf (stderr, "open failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

int Sendfile (int out_fd, int in_fd, off_t *offset, size_t count)
{
  int ret = 0;

  if ((ret = sendfile (out_fd, in_fd, offset, count)) == -1)
    {
      fprintf (stderr, "sendfile failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}
#endif //HPS_MYUTILI_H
