#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 5

int tab1[SIZE];
int tab2[SIZE];

void* mul(void* p){
  int i=*(int*)p;
  free(p);


  int* res=malloc(sizeof(int));
  *res=tab1[i]*tab2[i];

  pthread_exit(res);
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

  int sum=0;
  int** a=malloc(sizeof(int*));
  for(int i=0;i<SIZE;i++){
    
    pthread_join(idT[i],(void*)a);
    sum+=**a;
    free(*a);//on rend le pointeur alloc dans la fct
  }
  free(a);
  printf("%d\n",sum);
  

  return 0;
}
