/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FREQUENCY 100


/*No real reason to stop nicely, so SIGINT will be enough*/
int main(){

  if(getSemId()==-1){
    fprintf(stderr,"\nThe semaphore doesn't exist yet\n");
    fprintf(stderr,"Therefore, i don't any purpose, and i'm shutting down\n");
    exit(EXIT_FAILURE);
  }
  
  srand(time(NULL));

  while(1){
    
    if(rand()%FREQUENCY<(FREQUENCY/2)){/*If a new car comes in on this itération*/
      int got_a_spot=0;
      while(!got_a_spot){
	if(decSem_nowait(0)!=-1){
	  printf("Spot avaiable!\n");
	  printf("Printing ticket...\n");
	  printf("Spots remaining: %d\n",semctl(getSemId(),0,GETVAL));
	  got_a_spot=1;
	}
	else{
	  if(errno==EAGAIN){
	    printf("No spot remaining\n");
	    sleep(2);
	  }
	  else{
	    fprintf(stderr,"problem decSem_nowait : %s\n",strerror(errno));
	    fprintf(stderr,"Did the semaphore stop working?\n");
	    exit(EXIT_FAILURE);
	  }
	}
      }
    }
    else{
      sleep(1);
    }
  }
  
  exit(EXIT_FAILURE);
}
