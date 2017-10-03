
#define _GNU_SOURCE 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>


int i=0;

void * f1(void* p){
  //pthread_t moi = pthread_self();
  for(int i=0;i<4;i++){
    printf("I am %d!\n",++i);
  }
  return NULL;
}

int main(){
  pthread_t idTh;

  for(int i=0;i<10;i++){
    printf("i'm creating a new thread #%d\n",i);
    if(pthread_create(&idTh,NULL,f1,NULL) !=0){
      fprintf(stderr,"erreur de creation d'un thread\n");
      return 1;
    }
  }
  return 0;
}
