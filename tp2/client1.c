#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include "structCalculatorMessage.h"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>


int main(int argc,char** argv){
  if(argc!=4){
    printf("\nUsage : ./client1 number1 operator number2\n\n");
    return 1;
  }

  key_t k=ftok(MESSAGE_QUEUE_FILE_CALC,MESSAGE_QUEUE_FILE_PROG_ID);
  printf("key : %d\n",k);
  if(k==-1){
    printf("file doesn't exist for ftok\n");
  }
  int file=msgget(k,IPC_CREAT|0666);
  printf("file : %d\n",file);
  
  struct calculator_Message* buffer=malloc(sizeof(struct calculator_Message));
  
  if(calculator_Message_init(buffer,atoi(argv[1]),atoi(argv[3]),argv[2][0])){//not much verification, lazy.
    return 1;
  }

  if(-1==msgsnd(file,buffer,sizeof(struct Query),0)){
    fprintf(stderr,"problème msgsnd : %s.\n",strerror(errno));
    return 1;
  }
  else{
    printf("Query send\n");
  }

  if(-1==msgrcv(file,buffer,(size_t)sizeof(struct Query),1/* getpid()+4 */,0)){//offset of 4 on pid, since we have 4 op    
    fprintf(stderr,"problème msgrcv : %s.\n",strerror(errno));
    return 1;
  }

  printf("res : %d\n",buffer->q.res);
  
  free(buffer);

  return 0;
}
