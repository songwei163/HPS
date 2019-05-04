//
// Created by s on 19-5-4.
//

#include "myutili.h"

struct client_data {
  sockaddr_in address;
  char *write_buf;
  char buf[BUFFER_SIZE];
};

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

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  client_data *users = new client_data[FD_LIMIT];
  pollfd fds[USER_LIMIT + 1];
  int user_counter = 0;
  int ret = 0;
  for (int i = 1; i <= USER_LIMIT; ++i)
    {
      fds[i].fd = -1;
      fds[i].events = 0;
    }
  fds[0].fd = listenfd;
  fds[0].events = POLLIN | POLLERR;
  fds[0].revents = 0;

  while (1)
    {
      ret = poll (fds, user_counter + 1, -1);
      if (ret < 0)
        {
          printf ("poll failure\n");
          break;
        }

      for (int i = 0; i < user_counter + 1; ++i)
        {
          if ((fds[i].fd == listenfd) && (fds[i].revents & POLLIN))
            {
              struct sockaddr_in client_address;
              socklen_t client_addrlength = sizeof (client_address);
              int connfd = accept (listenfd, (struct sockaddr *) &client_address, &client_addrlength);
              if (connfd < 0)
                {
                  printf ("errno is: %d\n", errno);
                  continue;
                }
              if (user_counter >= USER_LIMIT)
                {
                  const char *info = "too many users\n";
                  printf ("%s", info);
                  send (connfd, info, strlen (info), 0);
                  close (connfd);
                  continue;
                }
              user_counter++;
              users[connfd].address = client_address;
              setnonblocking (connfd);
              fds[user_counter].fd = connfd;
              fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
              fds[user_counter].revents = 0;
              printf ("comes a new user, now have %d users\n", user_counter);
            }
          else if (fds[i].revents & POLLERR)
            {
              printf ("get an error from %d\n", fds[i].fd);
              char errors[100];
              memset (errors, '\0', 100);
              socklen_t length = sizeof (errors);
              if (getsockopt (fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &length) < 0)
                {
                  printf ("get socket option failed\n");
                }
              continue;
            }
          else if (fds[i].revents & POLLRDHUP)
            {
              users[fds[i].fd] = users[fds[user_counter].fd];
              close (fds[i].fd);
              fds[i] = fds[user_counter];
              i--;
              user_counter--;
              printf ("a client left\n");
            }
          else if (fds[i].revents & POLLIN)
            {
              int connfd = fds[i].fd;
              memset (users[connfd].buf, '\0', BUFFER_SIZE);
              ret = recv (connfd, users[connfd].buf, BUFFER_SIZE - 1, 0);
              printf ("get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd);
              if (ret < 0)
                {
                  if (errno != EAGAIN)
                    {
                      close (connfd);
                      users[fds[i].fd] = users[fds[user_counter].fd];
                      fds[i] = fds[user_counter];
                      i--;
                      user_counter--;
                    }
                }
              else if (ret == 0)
                {
                  printf ("code should not come to here\n");
                }
              else
                {
                  for (int j = 1; j <= user_counter; ++j)
                    {
                      if (fds[j].fd == connfd)
                        {
                          continue;
                        }

                      fds[j].events |= ~POLLIN;
                      fds[j].events |= POLLOUT;
                      users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
            }
          else if (fds[i].revents & POLLOUT)
            {
              int connfd = fds[i].fd;
              if (!users[connfd].write_buf)
                {
                  continue;
                }
              ret = send (connfd, users[connfd].write_buf, strlen (users[connfd].write_buf), 0);
              users[connfd].write_buf = NULL;
              fds[i].events |= ~POLLOUT;
              fds[i].events |= POLLIN;
            }
        }
    }

  delete[] users;
  close (listenfd);
  return 0;

}