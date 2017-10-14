#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "mqcalc.h"

void exit_del_mq(int mqid){
  //msgctl(mqid,IPC_RMID,NULL);
  exit(EXIT_FAILURE);
}

int main(int argc,char** argv){
  if(argc!=4){
    printf("\n Usage : ./query1 number1 operator number2\n\n");
    exit(EXIT_FAILURE);
  }

  key_t k=ftok(MQ_KEY_FILE,MQ_KEY_NUM);
  if(k==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));    exit(EXIT_FAILURE);;}else{printf("key : %d\n",k);}
  
  int mqid=-1;
  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){fprintf(stderr, "Problem msgget : %s.\n",strerror(errno));    exit(EXIT_FAILURE);;}else{printf("mqid : %d\n",mqid);}

  struct mq_calc msg;
  if(mq_calc_init(&msg,atoi(argv[1]),atoi(argv[3]),argv[2][0])){fprintf(stderr, "Operator not suported.\n");exit(EXIT_FAILURE);};
  
  if((msgsnd(mqid,&msg,sizeof(struct Query),0))==-1){fprintf(stderr, "Problem msgsnd : %s.\n",strerror(errno));exit(EXIT_FAILURE);};

  if(msgrcv(mqid,&msg,sizeof(struct Query),2,0)==-1){fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));exit(EXIT_FAILURE);};

  printf("Query result : %d\n",msg.q.res);
  return 0;
}
