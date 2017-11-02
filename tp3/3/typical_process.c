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
  int sleeep;
  
  srand(time(NULL));

  printf("process #%d reporting for duty\n",my_num);
  for(i=0;i<NB_AREA;++i){
    printf("trying to sub %d on semaphore #%d \n",my_num,i);
    if(subSem(i,my_num)==-1){fprintf(stderr,"Problem init subsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    sleeep=rand()%2;
    printf("process #%d gonna work for %d secondes on area #%d\n",my_num,sleeep,i);
    sleep(1);
    
    /*+1 so the next one can take it*/
    if(addSem(i,my_num+1)==-1){fprintf(stderr,"Problem init addsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  }

  /*a process is done, so it says so*/
  if(incSem(NB_SEM-1)==-1){fprintf(stderr,"Problem init incsem (wait) : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  printSem();
  printf("process #%d done working\n",my_num);
}

int main(int argc,char** argv){
  
  pid_t* children=malloc(NB_PROCESS*sizeof(pid_t)); 
  pid_t mainPID=getpid();
  int i;
  for(i=0;i<NB_PROCESS;++i){
    children[i]=fork();
    if(children[i]==0){/*I'm in the a son.*/
      free(children);/*Don't need them here.*/
      /*+1 because it'll be used as to control the sem*/
      ForkedFunc(i+1); 
      break;
    }
    else{
      printf("created process #%d\n",i);
    }
  }

 
  if(getpid()==mainPID){
    for(i=0;i<NB_PROCESS;i++){/*We wait for the childs to cleanly stop the process.*/
      wait(&children[i]);
      printf("Main catched a done process\n");
    }
    free(children);
  }   
  
  exit(EXIT_SUCCESS);
}
