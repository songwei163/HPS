//
// Created by s on 19-5-4.
//

#include "myutili.h"

struct fds
{
  int epollfd;
  int sockfd;
};

void addfd (int epollfd, int fd, bool oneshot)
{
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  if (oneshot)
    {
      event.events |= EPOLLONESHOT;
    }
  epoll_ctl (epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking (fd);
}

void reset_oneshot (int epollfd, int fd)
{
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
  epoll_ctl (epollfd, EPOLL_CTL_MOD, fd, &event);
}

void *worker (void *arg)
{
  int sockfd = ((fds *) arg)->sockfd;
  int epollfd = ((fds *) arg)->epollfd;
  printf ("start new thread to receive data on fd: %d\n", sockfd);
  char buf[BUFFER_SIZE];
  memset (buf, '\0', BUFFER_SIZE);
  while (1)
    {
      int ret = recv (sockfd, buf, BUFFER_SIZE - 1, 0);
      if (ret == 0)
        {
          close (sockfd);
          printf ("foreiner closed the connection\n");
          break;
        }
      else if (ret < 0)
        {
          if (errno == EAGAIN)
            {
              reset_oneshot (epollfd, sockfd);
              printf ("read later\n");
              break;
            }
        }
      else
        {
          printf ("get content: %s\n", buf);
          sleep (5);
        }
    }
  printf ("end thread receiving data on fd: %d\n", sockfd);
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

  epoll_event events[MAX_EVENT_NUMBER];
  int epollfd = epoll_create (5);
  assert(epollfd != -1);
  addfd (epollfd, listenfd, false);

  while (1)
    {
      int ret = epoll_wait (epollfd, events, MAX_EVENT_NUMBER, -1);
      if (ret < 0)
        {
          printf ("epoll failure\n");
          break;
        }

      for (int i = 0; i < ret; i++)
        {
          int sockfd = events[i].data.fd;
          if (sockfd == listenfd)
            {
              struct sockaddr_in client_address;
              socklen_t client_addrlength = sizeof (client_address);
              int connfd = accept (listenfd, (struct sockaddr *) &client_address, &client_addrlength);
              addfd (epollfd, connfd, true);
            }
          else if (events[i].events & EPOLLIN)
            {
              pthread_t thread;
              fds fds_for_new_worker;
              fds_for_new_worker.epollfd = epollfd;
              fds_for_new_worker.sockfd = sockfd;
              pthread_create (&thread, NULL, worker, (void *) &fds_for_new_worker);
            }
          else
            {
              printf ("something else happened \n");
            }
        }
    }

  close (listenfd);
  return 0;
}