//
// Created by s on 19-5-3.
//

#include "myutili.h"

static const char *status_line[2] = {"200 OK", "500 Internal server error"};

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

  const char *filename = argv[3];
  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  char header_buf[BUF_SIZE];
  memset (header_buf, '\0', BUF_SIZE);

  char *file_buf = NULL;
  struct stat file_stat;
  bool ret = true;
  int len = 0;

  char *file_name = argv[3];
  if (stat (file_name, &file_stat) == -1)
    {
      ret = false;
    }

  else
    {
      if (S_ISDIR (file_stat.st_mode))
        {
          ret = false;
        }
      else if (file_stat.st_mode & S_IROTH)
        {
          int fd = open (file_name, O_RDONLY);
          file_buf = new char[file_stat.st_size + 1];
          memset (file_buf, '\0', file_stat.st_size + 1);
          if (read (fd, file_buf, file_stat.st_size) < 0)
            {
              ret = false;
            }
        }
      else
        {
          ret = false;
        }
    }

  if (ret)
    {
      ret = snprintf (header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
      len += ret;
      ret = snprintf (header_buf + len, BUFFER_SIZE - 1 - len,
                      "Content-Length: %ld\r\n", file_stat.st_size);
      len += ret;
      ret = snprintf (header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
      struct iovec iv[2];
      iv[0].iov_base = header_buf;
      iv[0].iov_len = strlen (header_buf);
      iv[1].iov_base = file_buf;
      iv[1].iov_len = file_stat.st_size;
      ret = writev (connfd, iv, 2);
    }
  else
    {
      ret = snprintf (header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
      len += ret;
      ret = snprintf (header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
      send (connfd, header_buf, strlen (header_buf), 0);
    }
  close (connfd);
  delete[] file_buf;

  close (listenfd);
  return 0;
}

