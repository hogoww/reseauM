/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define FREQUENCY 200


/*No real reason to stop nicely, so SIGINT will be enough*/
int main(){

  if(getSemId()==-1){
    fprintf(stderr,"\nThe semaphore doesn't exist yet\n");
    fprintf(stderr,"Therefore, i don't any purpose, and i'm shutting down\n");
    exit(EXIT_FAILURE);
  }
  
  srand(time(NULL));

  while(1){
    /* 
       Not checking if we exceed maximum number of spot. impossible on a real program, since an event wouldn't be faked
       So, it's technicaly not right :)
    */

    if(rand()%FREQUENCY<(FREQUENCY/2)){/*If a new car comes in on this itération*/
      if(incSem(0)!=-1){
	printf("Someone left!\n");
	printf("Spots remaining: %d\n",semctl(getSemId(),0,GETVAL));
      }
      else{
	fprintf(stderr, "Problem incSem : %s.\n",strerror(errno));
      }
    }
    else{
      sleep(1);
    }
  }
  
  exit(EXIT_FAILURE);
}
