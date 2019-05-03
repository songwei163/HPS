//
// Created by s on 19-5-3.
//

/*
 * 与splice相比不消耗数据，源文件描述符上的数据仍可以用于后续操作
 */

#include "myutili.h"

int main (int argc, char *argv[])
{
  if (argc <= 1)
    {
      printf ("usage: %s file_name\n", basename (argv[0]));
      exit (EXIT_FAILURE);
    }

  int filefd = Open (argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);

  int pipefd_stdout[2];
  Pipe (pipefd_stdout);

  int pipefd_file[2];
  Pipe (pipefd_file);

  Splice (STDIN_FILENO, NULL, pipefd_stdout[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
  Tee (pipefd_stdout[0], pipefd_file[1], 32768, SPLICE_F_NONBLOCK);
  Splice (pipefd_file[0], NULL, filefd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
  Splice (pipefd_stdout[0], NULL, STDOUT_FILENO, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);

  close (filefd);
  close (pipefd_stdout[0]);
  close (pipefd_stdout[1]);
  close (pipefd_file[0]);
  close (pipefd_file[1]);
  return 0;
}
