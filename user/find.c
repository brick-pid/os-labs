#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void find(char *path, char *file)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  //open path(simple file or directory file)
  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  //stat path
  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
    if (strcmp(fmtname(path), file) == 0)
      printf("%s\n", path);
    break;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    //dirctory file is an array of dirent struct
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0)
      {
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      //don't descend into . and ..
      if (strlen(de.name) == 1 && de.name[0] == '.')
        continue;
      if (strlen(de.name) == 2 && de.name[0] == '.' && de.name[1] == '.')
        continue;

      find(buf, file);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc == 3)
  {
    find(argv[1], argv[2]);
  }
  exit(0);
}
