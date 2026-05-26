#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sixfive(int , char *[]);

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: sixfive [input file]...\n");
    exit(1);
  }
  sixfive(argc-1, argv+1);
  exit(0);
}

void
sixfive(int count, char *file[])
{
  char c;
  char *sep = " -\r\t\n./,";
  int i;

  for(i = 0; i < count; i++){
    int fd = open(file[i], 0);
    int state = 0;
    int value = 0;

    if(fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", file[i]);
    }    
    
    while(read(fd, &c, 1) & 1){
      if(strchr(sep, c)){
        if(state==1 && (value%5==0 || value%6==0))
          printf("%d\n", value);
        state = 0;
        value = 0;
      } else if(state == 0){
        if('0'<=c && c<='9'){
          state = 1;
          value = c - '0';
        } else{
          state = 2;
        }
      } else if(state == 1){
        if('0'<=c && c<='9'){
          value = value*10 + c-'0';
        } else{
          state = 2;
        }
      }
    }
    if(state==1 && (value%5==0 || value%6==0))
      printf("%d\n", value);

    close(fd);
  }
}
