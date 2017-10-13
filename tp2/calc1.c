#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include "structCalculatorMessage.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>


int file;


void sigINT_handler(int signo){
  if(signo == SIGINT){
    msgctl(file,IPC_RMID,NULL);//Deleting mq, we're done
    printf("exiting nicely :)\n");
    exit(EXIT_SUCCESS);
  }
}


int evalQuery(struct calculator_Message* mc);

int main(){


  if(signal(SIGINT, sigINT_handler) == SIG_ERR){
    fprintf(stderr,"attribution of function sig INT failed\n");
    exit(EXIT_FAILURE);
  }  

  key_t k=ftok(MESSAGE_QUEUE_FILE_CALC,MESSAGE_QUEUE_FILE_PROG_ID);
  if(k==-1){
    printf("file doesn't exist for ftok\n");
  }
  
  file=msgget(k,0666 | IPC_CREAT);
  if(file==-1){
    printf("file didn't work\n");
    return 1;
  }
  
  printf("file : %d\n",file);
  struct calculator_Message buffer;  

  while(1){
    printf("Waiting\n");
    if(-1==msgrcv(file,&buffer,(size_t)sizeof(struct Query),(long)0,0)){//Label 1 == query
      fprintf(stderr,"problème msgrcv : %s.\n",strerror(errno));
      return 1;
    }
    evalQuery(&buffer);
    calculator_Message_switchLabelToReturnLabel(&buffer);
    if(-1==msgsnd(file,&buffer,sizeof(struct Query),0)){
      fprintf(stderr,"problème msgsnd : %s.\n",strerror(errno));
      return 1;
    }
  }  

  return 0;
}


int evalQuery(struct calculator_Message* mc){//Very simple query, but i'm lazy.
  
  switch(mc->q.op){
  case CALC_ADD:
    mc->q.res=mc->q.n1+mc->q.n2;
    break;
  case CALC_SUB:
    mc->q.res=mc->q.n1-mc->q.n2;
    break;
  case CALC_MUL:
    mc->q.res=mc->q.n1*mc->q.n2;
    break;
  case CALC_DIV:
    mc->q.res=mc->q.n1/mc->q.n2;
    break;
  default:
    fprintf(stderr,"Query couldn't be evaluated\n");
    return 1;
  }
  return 0;
}
