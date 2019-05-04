//
// Created by s on 19-5-2.
//  自定义编写头文件
//

#ifndef HPS_MYUTILI_H
#define HPS_MYUTILI_H

#define LISTEN_QUEUE 5
#define BUF_SIZE 1024
#define BUFFER_SIZE 512
#define MAX_EVENT_NUMBER 1024

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535

#define MAX_EVENT_NUMBER 1024
#define TCP_BUFFER_SIZE 512
#define UDP_BUFFER_SIZE 1024

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
#include <poll.h>

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

int Open (const char *pathname, int flags, mode_t mode)
{
  int ret = 0;

  if ((ret = open (pathname, flags, mode)) == -1)
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

int Pipe (int pipefd[2])
{
  int ret = 0;

  if ((ret = pipe (pipefd)) == -1)
    {
      fprintf (stderr, "pipe failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

ssize_t Splice (int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags)
{
  ssize_t ret = 0;

  if ((ret == splice (fd_in, off_in, fd_out, off_out, len, flags)) == -1)
    {
      fprintf (stderr, "pipe failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

ssize_t Tee (int fd_in, int fd_out, size_t len, unsigned int flags)
{
  ssize_t ret = 0;

  if ((ret = tee (fd_in, fd_out, len, flags)) == -1)
    {
      fprintf (stderr, "tee failure, errno: %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return ret;
}

bool switch_to_user (uid_t user_id, gid_t gp_id)
{
  if ((user_id == 0) && (gp_id == 0))
    {
      return false;
    }

  gid_t gid = getgid ();
  uid_t uid = getuid ();
  if (((gid != 0) || (uid != 0)) && ((gid != gp_id) || (uid != user_id)))
    {
      return false;
    }

  if (uid != 0)
    {
      return true;
    }

  if ((setgid (gp_id) < 0) || (setuid (user_id) < 0))
    {
      return false;
    }

  return true;
}

bool daemonize ()
{
  pid_t pid = fork ();
  if (pid < 0)
    {
      return false;
    }
  else if (pid > 0)
    {
      exit (0);
    }

  umask (0);

  pid_t sid = setsid ();
  if (sid < 0)
    {
      return false;
    }

  if ((chdir ("/")) < 0)
    {
      /* Log the failure */
      return false;
    }

  close (STDIN_FILENO);
  close (STDOUT_FILENO);
  close (STDERR_FILENO);

  open ("/dev/null", O_RDONLY);
  open ("/dev/null", O_RDWR);
  open ("/dev/null", O_RDWR);
  return true;
}

int setnonblocking (int fd)
{
  int old_option = fcntl (fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl (fd, F_SETFL, new_option);
  return old_option;
}


#endif //HPS_MYUTILI_H
