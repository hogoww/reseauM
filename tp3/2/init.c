#include "semManip.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv){
  if(argc!=2){
   fprintf(stderr,"\n\n Use : ./init nb_process_waiting_for_eachover");
    exit(EXIT_FAILURE);
  }
  
  /*+1 for default value, since init have to wait too*/
  if(createSem(1,atoi(argv[1])+1)==-1){fprintf(stderr, "Problem createSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("Semaphore created, go nuts kids!\n");}
  
  /*unique wait*/
  if(waitSem(0)==-1){fprintf(stderr, "Problem waitSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  if(deleteSem()==-1){fprintf(stderr, "Problem deleteSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  exit(EXIT_SUCCESS);
}
