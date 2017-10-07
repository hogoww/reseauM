/*gcc -ansi -Wall -pedantic -o 2_2 2.2.c -lpthread -std=c99*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 5

int tab1[SIZE];
int tab2[SIZE];
int** tabR;

void* adding_thread(void* p){
  int* res=malloc(sizeof(int*));
  *res=0;
  for(int i=0;i<SIZE;i++){
    if(tabR[i]==NULL){
      sleep(1);//or while(1), both of them sucks anyway
    }
    else{
      *res+=*tabR[i];
      free(tabR[i]);
    }
  }
  free(tabR);
  pthread_exit((void*)res);
}

void* mul(void* p){
  int i=*(int*)p;
  free(p);

  tabR[i]=malloc(sizeof(int*));

  *tabR[i]=tab1[i]*tab2[i];

  pthread_exit(NULL);
}

void printTab(int t[]){
  for(int i=0;i<SIZE;i++){
    printf("%d ",t[i]);
  }
  printf("\n");
}


int main(){

  tabR=malloc(sizeof(int**)*SIZE);
  for(int i=0;i<SIZE;i++){//init rapidement vérifiable
    tab1[i]=i+1;
    tab2[i]=SIZE-i;
    tabR[i]=NULL;
  }
  printTab(tab1);
  printTab(tab2);
  

  pthread_t idT[SIZE+1];
  for(int i=0;i<SIZE;i++){
    int* j=malloc(sizeof(int*));
    *j=i;

    if(0!=pthread_create(&idT[i],NULL,mul,(void*)j)){
      fprintf(stderr,"Error while creating the #%d thread\n",i);
      return 1;
    }
  }
  
  if(0!=pthread_create(&idT[SIZE],NULL,adding_thread,NULL)){
    fprintf(stderr,"Error while creating the adding thread\n");
    return 1;
  }
  

  for(int i=0;i<SIZE;i++){
    pthread_join(idT[i],NULL);
  }

  int** res=malloc(sizeof(int*));
  pthread_join(idT[SIZE],(void*)res);

  printf("%d\n",**res);
  free(*res);
  free(res);

  return 0;
}
