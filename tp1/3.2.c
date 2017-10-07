/*gcc -ansi -Wall -pedantic -o 3_2 3.2.c -lpthread -std=c99*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 10

struct threadParameter{
  int* nbThreadTotal;
  int* nbThreadWaitingForSync;
  int doneCreating;

  pthread_cond_t doneCreatingCond;
  pthread_cond_t controlePoint;
  pthread_mutex_t mut;
  pthread_mutex_t mutCreation;
};

void initThreadParameter(struct threadParameter* p){
  p->nbThreadTotal=malloc(sizeof(int*));
  *(p->nbThreadTotal)=0;
  p->nbThreadWaitingForSync=malloc(sizeof(int*));
  *(p->nbThreadWaitingForSync)=0;
  p->doneCreating=0;
 
  pthread_cond_init(&(p->doneCreatingCond),NULL);
  pthread_cond_init(&(p->controlePoint),NULL);
  pthread_mutex_init(&(p->mut),NULL);
  pthread_mutex_init(&(p->mutCreation),NULL);
}

void destroyThreadParameter(struct threadParameter* p){
  free(p->nbThreadTotal);
  free(p->nbThreadWaitingForSync);
}

void* f(void*p){
  struct threadParameter* c=(struct threadParameter*)p;

  sleep(1);//workin on stuff
  
  printf("done on my first work\n");

  //First break point
  pthread_mutex_lock(&(c->mutCreation));
  while(1){
    if(!c->doneCreating){
      pthread_cond_wait(&(c->doneCreatingCond),&(c->mutCreation));
      break;
    }
    else{
      pthread_mutex_unlock(&(c->mutCreation));
      break;
    }
  }

  printf("going back to work\n");

  pthread_mutex_lock(&(c->mut));
  *(c->nbThreadWaitingForSync)+=1;

  while(1){//if we want to make another breakpoint, that's where we need to reset it.
    if(*(c->nbThreadWaitingForSync)!=*(c->nbThreadTotal)){
      pthread_cond_wait(&(c->controlePoint),&(c->mut));
      break;
    }
    else{
      c->nbThreadWaitingForSync=0;
      pthread_cond_signal(&(c->controlePoint));
      pthread_mutex_unlock(&(c->mut));
      break;
    }
  }
  pthread_mutex_unlock(&(c->mut));
  pthread_cond_broadcast(&(c->controlePoint));
  
  sleep(1);//workin
  
  
  printf("I'm done working, gonna go sleep\n");
  pthread_exit(NULL);
}

int main(){
  struct threadParameter* p=malloc(sizeof(struct threadParameter));
  initThreadParameter(p);
  

  pthread_t idT[SIZE];
  for(size_t i=0;i<SIZE;++i){
    pthread_mutex_lock(&(p->mut));
    if(pthread_create(&idT[i],NULL,f,(void*)p)){
      fprintf(stderr,"failed creating a thread\n");
      return 1;
    }
    *(p->nbThreadTotal)+=1;
    pthread_mutex_unlock(&(p->mut));
  }
  p->doneCreating=1;
  pthread_cond_broadcast(&(p->doneCreatingCond));
  
  for(size_t i=0;i<SIZE;++i){
    pthread_join(idT[i],NULL);
  }
  
  destroyThreadParameter(p);
  free(p);
  return 0;
}
