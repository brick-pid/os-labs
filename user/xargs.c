#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  char xargv[MAXARG][MAXARG]; //extended argv
  char *pass[MAXARG];
  int pid;
  int status;
  int i = argc - 1, j = 0;

  //copy argv[1], argv[2], ...... to xargv[0],......, not including argv[0] which is "xargs"
  for (int l = 0; l < argc - 1; l++)
  {
    strcpy(xargv[l], argv[l + 1]);
  }

  while (read(0, &xargv[i][j], sizeof(char)) > 0)
  {
    if (xargv[i][j] == ' ')
    {
      //split arguments
      xargv[i][j] = '\0';
      j = 0;
      i++;
    }
    else if (xargv[i][j] == '\n')
    {
      //exec command
      xargv[i][j] = '\0';
      for (int l = 0; l < MAXARG; l++)
      {
        pass[l] = xargv[l];
      }
      pass[i + 1] = 0;
      pid = fork();
      if (pid == 0)
      {
        //child process, exec command
        //char *test[] = {"echo", "hello", "world"};
        //exec("echo", test);

        exec(pass[0], pass);
        printf("my exec %s error\n", xargv[0]);
        exit(1);
      }
      else
      {
        //parent process
        wait(&status);
      }
      j = 0;
      i = argc - 1;
    }
    else
    {
      //non-white characters
      j++;
    }
  }

  exit(0);
}