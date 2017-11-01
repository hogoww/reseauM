/*
very little include because they are in semManip.h.
It's not a good practice, don't do it! 
*/

#include "semManip.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void ForkedFunc(int my_num){
  int i;

  srand(time(NULL));
  for(i=0;i<NB_AREA;i++){
    if(subSem(i,my_num)==-1){fprintf(stderr,"Problem init subsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    sleep(rand()%3);/*make it random*/
    
    /*+1 so the next one can take it*/
    if(addSem(i,my_num+1)==-1){fprintf(stderr,"Problem init subsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  }
}

int main(int argc,char** argv){
  
  pid_t* children=malloc(NB_PROCESS*sizeof(pid_t)); 
  pid_t mainPID=getpid();
  size_t i;
  for(i=0;i<NB_PROCESS;i++){
    children[i]=fork();
    if(children[i]==0){/*I'm in the a son.*/
      free(children);/*Don't need them here.*/
      ForkedFunc(i); 
      break;
    }
  }

 
  if(getpid()==mainPID){
    for(i=0;i<NB_PROCESS;i++){/*We wait for the childs to cleanly stop the process.*/
      wait(&children[i]);
    }
    free(children);
  }   
  
  exit(EXIT_SUCCESS);
}
