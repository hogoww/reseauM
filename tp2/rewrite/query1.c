#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "mqcalc.h"

int main(int argc,char** argv){
  if(argc!=4){
    printf("\n Usage : ./query1 number1 operator number2\n\n");
    return 1;
  }
  
  key_t k=ftok(MQ_KEY_FILE,MQ_KEY_NUM);
  if(k==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));return 1;}else{printf("key : %d\n",k);}
  
  int mqid=-1;
  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){printf("file:%d\n",mqid);}else{fprintf(stderr, "Problem msgget : %s.\n",strerror(errno));return 1;}

  struct mq_calc msg;
  if(mq_calc_init(&msg,atoi(argv[1]),atoi(argv[3]),argv[2][0])){fprintf(stderr, "Operator not suported : %s.\n",strerror(errno));return 1;};
  int e;
  if((e=msgsnd(mqid,(void *)&msg,sizeof(struct Query),0))==-1){fprintf(stderr, "Problem msgsnd : %s.%d\n",strerror(errno),e);return 1;};

  if(msgrcv(mqid,(void*)&msg,sizeof(struct Query),1,0)==-1){fprintf(stderr, "Operator not suported : %s.\n",strerror(errno));return 1;};

  printf("Query result : %d\n",msg.q.res);
  return 0;
}
