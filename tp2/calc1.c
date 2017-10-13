#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include "structCalculatorMessage.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>



int evalQuery(struct calculator_Message* mc);

int main(){
  
  key_t k=ftok(MESSAGE_QUEUE_FILE_CALC,MESSAGE_QUEUE_FILE_PROG_ID);
  if(k==-1){
    printf("file doesn't exist for ftok\n");
  }
 
  int file=msgget(k,IPC_CREAT|0666);  
  if(file==-1){
    printf("file didn't work\n");
    return 1;
  }
  
  /* struct msqid_ds* z=malloc(sizeof(struct msqid_ds)); */
  /* msgctl(file,IPC_STAT,z); */
  /* printf("%ld\n",z->msg_qbytes); */

  msgctl(file,IPC_RMID,NULL);//Reset file every time



  file=msgget(k,IPC_CREAT|0666);  
  if(file==-1){
    printf("file didn't work\n");
  }
  printf("%d\n",file);
  
  struct calculator_Message* buffer;
  buffer=malloc(sizeof(struct calculator_Message));
  while(1){
    printf("Waiting\n");
    if(-1==msgrcv(file,buffer,(long)sizeof(struct Query),0,0)){//Label 1 == query
      fprintf(stderr,"problème msgsnd : %s.\n",strerror(errno));
      return 1;
    }
    evalQuery(buffer);
    calculator_Message_switchLabelToReturnLabel(buffer);
    if(-1==msgsnd(file,buffer,sizeof(struct Query),0)){
      fprintf(stderr,"problème msgsnd : %s.\n",strerror(errno));
      return 1;
    }
  }  
  free(buffer);
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
