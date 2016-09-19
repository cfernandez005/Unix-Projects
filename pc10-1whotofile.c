//Assignment PC10-1, whotofile
//Chris Fernandez
//Editor: Notepad++
//Compiler: gcc
//Date: 3/5/14
//CS 3560 - Winter 2015

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "fcntl.h"

int main()
{
  int fd;
  printf( "\n\n  ::Booting who to userlist::\n\n" );

  switch (fork())
  {
    case -1:
      perror("fork");
      exit(2);
      
    case 0:
      if ( (fd = open("userlist", O_WRONLY|O_APPEND)) == -1 )
        fd = creat("userlist", 0644);
      
      if (dup2(fd, 1) == -1)
      {
        perror("Couldn't redirect stdout");
        exit(3);
      }
      close(fd);
      execlp("who", "who", NULL);
      perror("execlp");
      exit(1);
      
    default:
      wait(NULL);
      printf("  ::Mission complete, printing who::\n\n");
      fd = open("userlist", O_RDONLY);
      
      if (dup2(fd, 0) == -1)
      {
        perror("Couldn't redirect stdin");
        exit(3);
      }
      close(fd);
      execlp("more", "more", NULL);
      perror("execlp");
      exit(1);
  }
}
