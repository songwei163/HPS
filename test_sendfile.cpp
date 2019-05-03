//
// Created by s on 19-5-3.
//

/*
 * 发送文件
 */

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 3)
    {
      printf ("usage: %s ip_address port_number filename\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  const char *ip = argv[1];
  int port = atoi (argv[2]);

  int listenfd = Start_TCPserver_up (ip, port);

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_length = sizeof (cli_addr);
  int connfd = Accept (listenfd, (struct sockaddr *) &cli_addr, &cli_addr_length);

  const char *file_name = argv[3];
  int filefd = Open (file_name, O_RDONLY);
  struct stat stat_buf;
  fstat (filefd, &stat_buf);

  Sendfile (connfd, filefd, NULL, stat_buf.st_size);

  close (connfd);
  close (listenfd);
  return 0;
}



