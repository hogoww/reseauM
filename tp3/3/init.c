 /*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>

int main(){

  /*+1 for default value, since init have to wait too*/
  if(createSem()==-1){fprintf(stderr, "Problem createSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("Semaphore created, go nuts kids!\n");}
  
  /*-1 because semaphores start at 0*/
  if(subSem(NB_SEM-1,NB_PROCESS)==-1){fprintf(stderr,"Problem init subsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  printf("exiting nicely, have a nice day :)\n");
  if(deleteSem()==-1){fprintf(stderr,"Problem deleteSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  exit(EXIT_SUCCESS);
}
