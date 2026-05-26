#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2) {
    fprintf(2, "Usage: sleep [ticks num]\n");    
    exit(1);
  }
  
  int ticks = atoi(argv[1]);
  int ret = pause(ticks);
  exit(ret);
}
