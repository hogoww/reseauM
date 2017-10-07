/*gcc -ansi -Wall -pedantic -o 1 1.1.c -lpthread -std=c99*/
 
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>


int i=0;

void * f1(void* p){
  pthread_exit(NULL);
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
  //Not waiting for them because that exercices is useless.
  return 0;
}
