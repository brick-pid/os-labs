#include "kernel/types.h"
#include "user/user.h"

int main()
{
  int pToChild[2]; //p[0] for read, p[1] for write
  int pToParent[2];
  int pid;

  pipe(pToChild);
  pipe(pToParent);

  pid = fork();
  if (pid == 0)
  {
    //child process
    int n;
    char buf[8];
    close(pToChild[1]);
    close(pToParent[0]);

    n = read(pToChild[0], buf, sizeof(buf));
    if (n == 1)
    {
      printf("%d: received ping\n", getpid());
    }
    write(pToParent[1], buf, 1);

    close(pToChild[0]);
    close(pToParent[1]);

    exit(0);
  }
  else
  {
    //parent process
    int n;
    char buf[8];
    close(pToChild[0]);
    close(pToParent[1]);

    write(pToChild[1], buf, 1);

    n = read(pToParent[0], buf, 1);
    if (n == 1)
    {
      printf("%d: received pong\n", getpid());
    }

    close(pToChild[1]);
    close(pToParent[0]);

    exit(0);
  }
}