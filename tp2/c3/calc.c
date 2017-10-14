#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include "mqcalc.h"

int mqid;

void sigINT_handler(int signo){
  if(signo==SIGINT){
    msgctl(mqid,IPC_RMID,NULL);//Delete mq on leave
    printf("exiting nicely :) \n");
    exit(EXIT_SUCCESS);
  }
}

void evalQuery(struct mq_calc* query);

void* calcSub(void* p);//One for each operator (not needed if we pass parameter but... Lazyness, my dearest...
void* calcMul(void* p);
void* calcDiv(void* p);

void* calcAdd(void* p){
  int whoAmI=MQ_C_ADD;
  struct mq_calc query;
  while(1){
    //printf("waiting\n");
    if(msgrcv(mqid,&query,(size_t)sizeof(struct Query),whoAmI,0)==-1)
      {fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
    
    evalQuery(&query);
    mq_calc_putReturnLabel(&query);
    
    if(msgsnd(mqid,&query,(size_t)sizeof(struct Query),0)==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
  }

  pthread_exit(NULL);//Not supposed to go there.
}
  

int main(){
  if(signal(SIGINT,sigINT_handler)==SIG_ERR){fprintf(stderr,"attribution of SIGIN trap failed\n");exit(EXIT_FAILURE);}

  key_t k=ftok(MQ_KEY_FILE,MQ_KEY_NUM);
  if(k==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));sigINT_handler(SIGINT);}else{printf("key : %d\n",k);}

  mqid=-1;
  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){fprintf(stderr, "Problem msggets : %s.\n",strerror(errno));sigINT_handler(SIGINT);}else{printf("mqid : %d\n",mqid);}

  pthread_t idt[4];//idthread, for th join // worker 0 on picture is the main.
  pthread_create(&idt[0],NULL,calcAdd,NULL);
  pthread_create(&idt[1],NULL,calcSub,NULL);
  pthread_create(&idt[2],NULL,calcMul,NULL);
  pthread_create(&idt[3],NULL,calcDiv,NULL);
  //We should check if the creation of any of them is failing. Which i'm too lazy for.

  while(1){
    sleep(1);//Main doesn't do anything, because... because i said so !
  }
  //Not pthread_join either, since we're stopping them with a SIGINT
  
  
  exit(EXIT_FAILURE);//Not supposed to go there
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

void* calcSub(void* p){
  int whoAmI=MQ_C_SUB;
  struct mq_calc query;
  while(1){
    //printf("waiting\n");
    if(msgrcv(mqid,&query,(size_t)sizeof(struct Query),whoAmI,0)==-1)
      {fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
    
    evalQuery(&query);
    mq_calc_putReturnLabel(&query);
    
    if(msgsnd(mqid,&query,(size_t)sizeof(struct Query),0)==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
  }

  pthread_exit(NULL);//Not supposed to go there.
}

void* calcMul(void* p){
  int whoAmI=MQ_C_MUL;
  struct mq_calc query;
  while(1){
    //printf("waiting\n");
    if(msgrcv(mqid,&query,(size_t)sizeof(struct Query),whoAmI,0)==-1)
      {fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
    
    evalQuery(&query);
    mq_calc_putReturnLabel(&query);
    
    if(msgsnd(mqid,&query,(size_t)sizeof(struct Query),0)==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
  }

  pthread_exit(NULL);//Not supposed to go there.
}

void* calcDiv(void* p){
  int whoAmI=MQ_C_DIV;
  struct mq_calc query;
  while(1){
    //printf("waiting\n");
    if(msgrcv(mqid,&query,(size_t)sizeof(struct Query),whoAmI,0)==-1)
      {fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
    
    evalQuery(&query);
    mq_calc_putReturnLabel(&query);
    
    if(msgsnd(mqid,&query,(size_t)sizeof(struct Query),0)==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));sigINT_handler(SIGINT);}
  }

  pthread_exit(NULL);//Not supposed to go there.
}
