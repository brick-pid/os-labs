#include "kernel/types.h"
#include "user/user.h"

int main()
{

  if (fork() == 0)
  {
    for (int i = 0; 1; i++)
    {
      if (i % 100000 == 0)
        printf("/");
    }
  }
  else
  {
    for (int i = 0; 1; i++)
    {
      if (i % 100000 == 0)
        printf("\\");
    }
  }
  return 0;
}