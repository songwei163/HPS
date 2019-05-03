//
// Created by s on 19-5-3.
//

#include <unistd.h>
#include <stdio.h>

int main()
{
  uid_t uid = getuid();
  uid_t euid = geteuid();
  printf( "userid is %d, effective userid is: %d\n", uid, euid );
  return 0;
}

