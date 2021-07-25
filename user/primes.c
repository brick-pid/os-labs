#include "kernel/types.h"
#include "user/user.h"

int main()
{
  int p[2];
  int pid, status = 0, cnt = 34; //first write 34 number(from 2 to 35)
  if (pipe(p) == -1)
  {
    printf("error opening pipe");
    exit(1);
  }
  pid = fork();

  if (pid == 0)
  {
  //child process
  label:
    while (cnt > 0)
    {
      close(p[1]); //close write end
      int first;   //first read number is prime
      int num;
      int buf[34]; //numbers to be written

      cnt = 0;
      read(p[0], &first, sizeof(first));
      printf("prime %d\n", first);
      while (read(p[0], &num, sizeof(num)) > 0)
      {
        if (num % first != 0)
        {
          //num is a prime
          buf[cnt] = num;
          cnt++;
        }
      }

      close(p[0]); //close read end to the previous pipe
      //-----end reading from left process, begin writing to right process

      if (cnt > 0)
      {
        pipe(p);
        pid = fork();
      }
      else
      {
        //this is the last process, ending the pipe line
        exit(0);
      }

      //-----begin the next process
      if (pid == 0)
      {
        //right process
        goto label;
      }
      else
      {
        //left process
        close(p[0]);
        for (int i = 0; i < cnt; i++)
        {
          write(p[1], &buf[i], sizeof(int));
        }
        close(p[1]);
        wait(&status);
        exit(0);
      }
    }
  }
  else
  {
    //gardian process
    close(p[0]); //close read end
    for (int i = 2; i < 36; i++)
    {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]); //close write end
    wait(&status);
    exit(0);
  }
  return 0;
}