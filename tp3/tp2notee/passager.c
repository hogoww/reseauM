#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

#define FILE_KEY "init.c"
#define INT_KEY 5

int main(int argc,char** argv){
  key_t key;
  int semid;
  struct sembuf w;
  
  if((key=ftok(FILE_KEY,INT_KEY))==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  if((semid=semget(key,5,0600))==1){fprintf(stderr, "Problem semget : %s.\n Peut être que la sémaphore n'as pas encore été créée?",strerror(errno));exit(EXIT_FAILURE);}
  
  
  do{
    /*On attend sur la semaphore #1 que bus ai de la place && qu'il soit vide.*/
    w.sem_num=1;
    w.sem_op=0;
    w.sem_flg=0;
    if(semop(semid,&w,1)==-1){fprintf(stderr, "Problem wait sem : %s.\n Avez vous enlevé la semaphore alors que le bus contenais quelques personnes?\n",strerror(errno));exit(EXIT_FAILURE);}

    /*Le bus a de la place, on essaye d'y rentrer..*/
    w.sem_num=0;
    w.sem_op=-1;
    w.sem_flg=IPC_NOWAIT;
    if((semop(semid,&w,1)==-1) && errno!=EAGAIN){fprintf(stderr, "Problem en rentrant dans le bus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  }while(errno==EAGAIN);/*Il n'y avait plus de place quand on à essayer d'y rentrer*/
    
  printf("j'ai réussi à rentrer dans le bus\n");
      
  /*On attend que le bus parte*/
  w.sem_num=3;
  w.sem_op=0;
  w.sem_flg=0;
  if(semop(semid,&w,1)==-1){fprintf(stderr, "Probleme en attendant que le bus parte : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  printf("Le bus est partis\n");

  /*On attend pour sortir du bus*/
  w.sem_num=2;
  w.sem_op=0;
  w.sem_flg=0;
  if(semop(semid,&w,1)==-1){fprintf(stderr, "Probleme en essayant de descendre du bus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    
  printf("j'attends pour sortir du bus\n");
  
  /*Et on sort du bus*/
  w.sem_num=0;
  w.sem_op=1;
  w.sem_flg=0;
  if(semop(semid,&w,1)==-1){fprintf(stderr, "Probleme en essayant de descendre du bus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  printf("je suis sortis du bus\n");

  exit(EXIT_SUCCESS);
}
