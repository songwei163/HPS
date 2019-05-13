//
// Created by s on 19-5-13.
//

/*
 * 统一事件源
 */

#include "myutili.h"

static int pipefd[2];

void addfd (int epollfd, int fd)
{
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl (epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking (fd);
}

void sig_handler (int sig)
{
  int save_errno = errno;
  int msg = sig;
  send (pipefd[1], (char *) &msg, 1, 0);
  errno = save_errno;
}

void addsig (int sig)
{
  struct sigaction sa;
  memset (&sa, '\0', sizeof (sa));
  sa.sa_handler = sig_handler;
  sa.sa_flags |= SA_RESTART;
  sigfillset (&sa.sa_mask);
  assert (sigaction (sig, &sa, NULL) != -1);
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

  int listenfd = Start_TCPserver_up (ip, port);
  int reuse = 1;
  setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse));

  epoll_event events[MAX_EVENT_NUMBER];
  int epollfd = Epoll_create (5);
  assert(epollfd != -1);
  addfd (epollfd, listenfd);

  /*使用socketpair创建管道，注册pipefd[0]上的可读事件*/
  Socketpair (PF_UNIX, SOCK_STREAM, 0, pipefd);
  setnonblocking (pipefd[1]);
  addfd (epollfd, pipefd[0]);

  /*设置一些信号处理函数*/
  addsig (SIGHUP);
  addsig (SIGCHLD);
  addsig (SIGTERM);
  addsig (SIGINT);

  bool stop_server = false;
  int ret = 0;

  while (!stop_server)
    {
      int number = epoll_wait (epollfd, events, MAX_EVENT_NUMBER, -1);
      if ((number < 0) && (errno != EINTR))
        {
          printf ("epoll failure\n");
          break;
        }
      for (int i = 0; i < number; ++i)
        {
          int sockfd = events[i].data.fd;

          if (sockfd == listenfd)
            {
              struct sockaddr_in client_address;
              socklen_t client_addrlength = sizeof (client_address);
              int connfd = Accept (listenfd, (struct sockaddr *) &client_address, &client_addrlength);
              addfd (epollfd, connfd);
            }
          else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN))
            {
              int sig;
              char signals[1024];
              ret = recv (pipefd[0], signals, sizeof (signals), 0);

              if (ret == -1)
                {
                  continue;
                }
              else if (ret == 0)
                {
                  continue;
                }
              else
                {
                  for (int i = 0; i < ret; ++i)
                    {
                      switch (signals[i])
                        {
                          case SIGCHLD:
                          case SIGHUP:
                            {
                              continue;
                            }
                          case SIGTERM:
                          case SIGINT:
                            {
                              stop_server = true;
                            }
                        }
                    }
                }
            }
          else
            {

            }
        }
    }

  printf ("close fds\n");
  close (listenfd);
  close (pipefd[1]);
  close (pipefd[0]);
  return 0;
}