/*gcc -ansi -Wall -pedantic -o 4_2 4.2.c -lpthread -std=c99*/
#include <stdlib.h>//Free()
#include <stdio.h>
#include <pthread.h>//Everything §
#include <unistd.h>//sleep()

//How much task we do at the same time
#define THREAD_NUMBER 4
//Dummy var, since we simulate the picture
#define NUMBER_OF_AREA 10

void* f1(void*);
void* f2(void*);
void* f3(void*);
void* f4(void*);


struct thread_data{
  size_t last_thread_on_that_zone[NUMBER_OF_AREA];
  int picture[NUMBER_OF_AREA];//No uses, just for coherence
  pthread_mutex_t* mutex_data;
  pthread_cond_t* vcond_data;
};

void init_thread_data(struct thread_data* p){
  for(size_t i=0;i<THREAD_NUMBER;++i){
    p->last_thread_on_that_zone[i]=0;
  }
 
  p->mutex_data=malloc(sizeof(pthread_mutex_t)*NUMBER_OF_AREA);
  p->vcond_data=malloc(sizeof(pthread_cond_t)*NUMBER_OF_AREA);
  
  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_init(&(p->mutex_data[i]),NULL);
    pthread_cond_init(&(p->vcond_data[i]),NULL);
  }
}

void destroy_thread_data(struct thread_data* p){
  free(p->mutex_data);
  free(p->vcond_data);
}


void* f1(void* p){//Since we don't do actual work, the others are the same, therefore copy/paste after the main
  struct thread_data* param=(struct thread_data*)p;//To cast only once

  size_t my_function_number=1;
  
  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_lock(&(param->mutex_data[i]));
    while(param->last_thread_on_that_zone[i]!=my_function_number){
      pthread_cond_wait(&(param->vcond_data[i]),&(param->mutex_data[i]));//The previous thread isn't done with that area, we wait and give back the lock
    }
    
    printf("thread number %zu proudly working on area #%zu\n",my_function_number,i);
    sleep(.5);//Working on the i'th area
    param->last_thread_on_that_zone[i]++;//So the first thread can take that area.
    pthread_cond_broadcast(&(param->vcond_data[i]));//Let everyone know that we're done with that area.//It's (uselessly) left in the last thread too.
    pthread_mutex_unlock(&(param->mutex_data[i]));//Done with that area.
  }
  pthread_exit(NULL);
}


int main(){
  struct thread_data* param;
  param=malloc(sizeof(struct thread_data));
  init_thread_data(param);
  
  pthread_t idt[THREAD_NUMBER];//idthread, for th join // worker 0 on picture is the main.
  pthread_create(&idt[0],NULL,f1,(void*)param);
  pthread_create(&idt[1],NULL,f2,(void*)param);
  pthread_create(&idt[2],NULL,f3,(void*)param);
  pthread_create(&idt[3],NULL,f4,(void*)param);
  //We should check if the creation of any of them is failing. Which i'm too lazy for.

  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_lock(&(param->mutex_data[i]));//Main is the first one, so no need to check if he can work on the area
    sleep(.5);//Working on the i'th area
    printf("main thread proudly working on area #%zu\n",i);
    param->last_thread_on_that_zone[i]++;//So the first thread can take that area
    pthread_cond_broadcast(&(param->vcond_data[i]));//Let everyone know that we're done with that area.
    pthread_mutex_unlock(&(param->mutex_data[i]));//Done with that area
  }
  
  printf("main is done and waiting for his slow coworkers is finally done!\n");
  
  for(size_t i=0;i<THREAD_NUMBER;++i){
    pthread_join(idt[i],NULL);
    printf("thread #%zu is finally done!\n",i+1);
  }
  
  destroy_thread_data(param);
  free(param);
  return 0;
}


void* f2(void* p){//Since we don't do actual work, the others are the same, therefore copy/paste after the main
  struct thread_data* param=(struct thread_data*)p;//To cast only once

  size_t my_function_number=2;
  
  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_lock(&(param->mutex_data[i]));
    while(param->last_thread_on_that_zone[i]!=my_function_number){
      pthread_cond_wait(&(param->vcond_data[i]),&(param->mutex_data[i]));//The previous thread isn't done with that area, we wait and give back the lock
    }
    
    printf("thread number %zu proudly working on area #%zu\n",my_function_number,i);
    sleep(.5);//Working on the i'th area
    param->last_thread_on_that_zone[i]++;//So the first thread can take that area.
    pthread_cond_broadcast(&(param->vcond_data[i]));
    pthread_mutex_unlock(&(param->mutex_data[i]));//Done with that area.
  }
  pthread_exit(NULL);
}


void* f3(void* p){//Since we don't do actual work, the others are the same, therefore copy/paste after the main
  struct thread_data* param=(struct thread_data*)p;//To cast only once

  size_t my_function_number=3;
  
  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_lock(&(param->mutex_data[i]));
    while(param->last_thread_on_that_zone[i]!=my_function_number){
      pthread_cond_wait(&(param->vcond_data[i]),&(param->mutex_data[i]));//The previous thread isn't done with that area, we wait and give back the lock
    }
    
    printf("thread number %zu proudly working on area #%zu\n",my_function_number,i);
    sleep(.5);//Working on the i'th area
    param->last_thread_on_that_zone[i]++;//So the first thread can take that area.
    pthread_cond_broadcast(&(param->vcond_data[i]));
    pthread_mutex_unlock(&(param->mutex_data[i]));//Done with that area.
  }
  pthread_exit(NULL);
}


void* f4(void* p){//Since we don't do actual work, the others are the same, therefore copy/paste after the main
  struct thread_data* param=(struct thread_data*)p;//To cast only once

  size_t my_function_number=4;
  
  for(size_t i=0;i<NUMBER_OF_AREA;++i){
    pthread_mutex_lock(&(param->mutex_data[i]));
    while(param->last_thread_on_that_zone[i]!=my_function_number){
      pthread_cond_wait(&(param->vcond_data[i]),&(param->mutex_data[i]));//The previous thread isn't done with that area, we wait and give back the lock
    }
    
    printf("thread number %zu proudly working on area #%zu\n",my_function_number,i);
    sleep(.5);//Working on the i'th area
    param->last_thread_on_that_zone[i]++;//So the first thread can take that area.
    pthread_cond_broadcast(&(param->vcond_data[i]));
    pthread_mutex_unlock(&(param->mutex_data[i]));//Done with that area.
  }
  pthread_exit(NULL);
}
