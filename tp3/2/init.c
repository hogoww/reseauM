/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char** argv){
  int nb_process;
    if(argc!=2){
      fprintf(stderr,"\nUse : ./init nb_process_waiting_for_eachover\n");
      exit(EXIT_FAILURE);
    }
  
  nb_process=atoi(argv[1]);
  /*+1 for default value, since init have to wait too*/
  if(createSem(nb_process+1)==-1){fprintf(stderr, "Problem createSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("Semaphore created, go nuts kids!\n");}
  
  /*unique wait*/
  if(waiting()==-1){fprintf(stderr,"Problem waitSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("Done waiting, Gonna remove that semaphore before going to sleep!\n");}
  
  if(subSem(1,nb_process+1)==-1){fprintf(stderr,"Problem waitSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  if(deleteSem()==-1){fprintf(stderr,"Problem deleteSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  exit(EXIT_SUCCESS);
}
