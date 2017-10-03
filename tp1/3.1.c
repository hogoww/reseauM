#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 10

struct threadParameter{
  int* nbThreadTotal;
  int* nbThreadWaitingForSync;
  int doneCreating;
  pthread_mutex_t mut;
};

void initThreadParameter(struct threadParameter* p){
  p->nbThreadTotal=malloc(sizeof(int*));
  *(p->nbThreadTotal)=0;
  p->nbThreadWaitingForSync=malloc(sizeof(int*));
  *(p->nbThreadWaitingForSync)=0;
  p->doneCreating=0;

  pthread_mutex_init(&(p->mut),NULL);
}

void destroyThreadParameter(struct threadParameter* p){
  free(p->nbThreadTotal);
  free(p->nbThreadWaitingForSync);
}

void* f(void*p){
  struct threadParameter* c=(struct threadParameter*)p;
  //sleep(1);
  while(!c->doneCreating){ 
    sleep(1);
  }  

  printf("I'm done working, i'm waiting for my slow brothers\n");

  pthread_mutex_lock(&(c->mut));
  *(c->nbThreadWaitingForSync)+=1;
  pthread_mutex_unlock(&(c->mut));



  while(*(c->nbThreadWaitingForSync)!=*(c->nbThreadTotal)){ //if we want to make another breakpoint, that's where we need to reset it.
    sleep(1);
  }

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
  
  for(size_t i=0;i<SIZE;++i){
    pthread_join(idT[i],NULL);
  }
  
  destroyThreadParameter(p);
  free(p);
  return 0;
}
