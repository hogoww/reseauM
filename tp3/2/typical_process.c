/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>

int main(){

  if((waiting())==-1){
    fprintf(stderr,"\nSemaphore doesn't exist yet\n");
    fprintf(stderr,"Therefore, i don't any purpose, and i'm shutting down\n");
    exit(EXIT_FAILURE);
  }

  printf("Done waiting, gonna do some work !\n");
  sleep(1);
  printf("Done working, gonna go to sleep! !\n");
  
  exit(EXIT_SUCCESS);
}
