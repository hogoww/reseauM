/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


void sigINT_handler(int signo){
  if(signo==SIGINT){
    if(deleteSem()==-1){fprintf(stderr,"Problem deleteSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}    
    printf("exiting nicely :), any ticket machine remaining will crash.\n");
    exit(EXIT_SUCCESS);
  }
}


int main(int argc,char** argv){
  int nb_spot_parking;
  if(argc!=2){
    fprintf(stderr,"\n\n Use : ./init nb_spot_parking\n");
    exit(EXIT_FAILURE);
  }
  nb_spot_parking=atoi(argv[1]);
  if(nb_spot_parking<1){
    fprintf(stderr,"I don't know what a parking with a negative number of spot is.");
  }

  if(signal(SIGINT,sigINT_handler)==SIG_ERR){fprintf(stderr,"attribution of SIGIN trap failed\n");exit(EXIT_FAILURE);}
  
  

  if(createSem(nb_spot_parking)==-1){fprintf(stderr, "Problem createSem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("Semaphore created, go nuts kids!\n");}
  
  /*unique wait*/
  while(1){};/*We need to keep the Parking going forever and ever ! But in case we want to stop, we can use SIGINT*/
  
  exit(EXIT_FAILURE);/*Never suppose to get there*/
}
