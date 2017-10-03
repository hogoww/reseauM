#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 5

int tab1[SIZE];
int tab2[SIZE];
int sum=0;
pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;

void* mul(void* p){
  int i=*(int*)p;
  free(p);

  int t=tab1[i]*tab2[i];

  pthread_mutex_lock(&mut);
  sum+=t;
  pthread_mutex_unlock(&mut);

  pthread_exit(NULL);
}

void printTab(int t[]){
  for(int i=0;i<SIZE;i++){
    printf("%d ",t[i]);
  }
  printf("\n");
}


int main(){
  for(int i=0;i<SIZE;i++){//init rapidement vérifiable
    tab1[i]=i+1;
    tab2[i]=SIZE-i;
  }
  printTab(tab1);
  printTab(tab2);
  

  pthread_t idT[SIZE];
  for(int i=0;i<SIZE;i++){
    int* j=malloc(sizeof(int));
    *j=i;
    if(0!=pthread_create(&idT[i],NULL,mul,j)){
      fprintf(stderr,"Error while creating the #%d thread\n",i);
      return 1;
    }
  }

  for(int i=0;i<SIZE;i++){
    pthread_join(idT[i],NULL);
  }

  printf("%d\n",sum);
  pthread_mutex_destroy(&mut);

  return 0;
}
