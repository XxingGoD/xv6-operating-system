#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

static char *exec_argv[MAXARG];
static int exec_argc;

void
runexec(char *path)
{
  int pid, status;

  if(exec_argc == 0){
    printf("%s\n", path);
    return;
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0){
    exec_argv[exec_argc] = path;
    exec_argv[exec_argc + 1] = 0;
    exec(exec_argv[0], exec_argv);
    fprintf(2, "find: exec %s failed\n", exec_argv[0]);
    exit(1);
  }
  wait(&status);
}

void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type == T_FILE){
    char *filename = path;
    for(char *q = path; *q; q++){
      if(*q == '/')
        filename = q + 1;
    }
    if(strcmp(filename, name) == 0)
      runexec(path);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if(stat(buf, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    if(strcmp(de.name, name) == 0)
      runexec(buf);

    if(st.type == T_DIR)
      find(buf, name);
  }

  close(fd);

}

int 
main(int argc, char *argv[])
{
  int i;

  if(argc < 3){
    fprintf(2, "Usage: find path file [-exec cmd ...]\n");
    exit(1);
  }

  exec_argc = 0;
  if(argc > 3){
    if(strcmp(argv[3], "-exec") != 0){
      fprintf(2, "Usage: find path file [-exec cmd ...]\n");
      exit(1);
    }
    for(i = 4; i < argc && exec_argc < MAXARG - 2; i++)
      exec_argv[exec_argc++] = argv[i];
    if(exec_argc == 0){
      fprintf(2, "find: missing command after -exec\n");
      exit(1);
    }
  }

  find(argv[1], argv[2]);
  exit(0);
}
