#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "mqcalc.h"

int mqid;

void sigINT_handler(int signo){
  if(signo==SIGINT){
    msgctl(mqid,IPC_RMID,NULL);
    printf("exiting nicely :) \n");
    exit(EXIT_SUCCESS);
  }
}

void evalQuery(struct mq_calc* query);


int main(){
  if(signal(SIGINT,sigINT_handler)==SIG_ERR){fprintf(stderr,"attribution of SIGIN trap failed\n");exit(EXIT_FAILURE);}

  key_t k=ftok(MQ_KEY_FILE,MQ_KEY_NUM);
  if(k==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));sigINT_handler(SIGINT);}else{printf("key : %d\n",k);}

  mqid=-1;
  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){fprintf(stderr, "Problem msggets : %s.\n",strerror(errno));sigINT_handler(SIGINT);}else{printf("mqid : %d\n",mqid);}

  struct mq_calc query;
  while(1){
    printf("waiting\n");
    if(msgrcv(mqid,&query,(size_t)sizeof(struct Query),1,0)==-1)
      {fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
    
    evalQuery(&query);
    mq_calc_putReturnLabel(&query);

    if(msgsnd(mqid,&query,(size_t)sizeof(struct Query),0)==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
  }
  
  exit(EXIT_FAILURE);
}


void evalQuery(struct mq_calc* mc){
  switch(mc->q.op){
  case MQ_C_ADD:
    mc->q.res=mc->q.n1+mc->q.n2;
    break;
  case MQ_C_SUB:
    mc->q.res=mc->q.n1-mc->q.n2;
    break;
  case MQ_C_MUL:
    mc->q.res=mc->q.n1*mc->q.n2;
    break;
  case MQ_C_DIV:
    mc->q.res=mc->q.n1/mc->q.n2;
    break;
  default:
    mc->q.res=-1;
    fprintf(stderr,"Eval query failed");
    break;
  }
}
