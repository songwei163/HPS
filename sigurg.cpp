//
// Created by s on 19-5-13.
//

/*
 * Linux环境下，内核通知应用程序带外数据到达主要有两种方法：
 *  1>  I/O复用技术，select等系统调用接收到带外数据时返回，并向应用程序报告socket上的异常事件
 *  2>  使用SIGURG信号
 */

#include "myutili.h"

static int connfd;

void sig_urg (int sig)
{
  int save_errno = errno;
  char buffer[BUF_SIZE];
  memset (buffer, '\0', BUF_SIZE);
  int ret = Recv (connfd, buffer, BUF_SIZE - 1, MSG_OOB);
  printf ("got %d bytes of oob data: '%s'\n", ret, buffer);
  errno = save_errno;
}

void addsig (int sig, void(*sig_handler) (int))
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

  struct sockaddr_in client_addr;
  socklen_t client_addrlength = sizeof (client_addr);
  connfd = accept (listenfd, (struct sockaddr *) &client_addr, &client_addrlength);
  if (connfd < 0)
    {
      printf ("errno is : %d\n", errno);
    }
  else
    {
      addsig (SIGURG, sig_urg);

      /*使用SIGURG信号之前，我们必须设置socket的宿主进程或进程组*/
      fcntl (connfd, F_SETOWN, getpid ());

      char buffer[BUF_SIZE];
      while (1)
        {
          memset (buffer, '\0', BUF_SIZE);
          int ret = Recv (connfd, buffer, BUF_SIZE - 1, 0);
          if (ret <= 0)
            {
              break;
            }
          printf ("got %d bytes of normal data '%s'\n", ret, buffer);
        }
      close (connfd);
    }
  close (listenfd);
  return 0;
}