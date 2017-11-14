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


union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};


int main(int argc,char** argv){
  key_t key;
  int semid;
  union semun u;
  int initialValue;
  struct sembuf w;
  
  
  if(argc!=2){
    fprintf(stderr,"\n\n Usage : ./init NOMBRE_DE_PASSAGER\n\n");
  }
  
  if((key=ftok(FILE_KEY,INT_KEY))==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  /*Deux numero de semaphore, un pour compter les passager, l'autre pour finir ce processus.*/
  if((semid=semget(key,5,0600 | IPC_CREAT))==-1){fprintf(stderr, "Problem semget : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  initialValue=atoi(argv[1]);
  u.val=initialValue;
  if(semctl(semid,0,SETVAL,u)==-1){fprintf(stderr, "Problem semctl init value nombre_passager : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  /*attendras que bus mette la valeur à 0 pour laisser monter les gens.*/
  u.val=1;
  if(semctl(semid,1,SETVAL,u)==-1){fprintf(stderr, "Problem semctl init value monter dans le bus possible : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  /*attendras que bus mette la valeur à 0 pour laisser les gens sortir.*/
  u.val=0;
  if(semctl(semid,2,SETVAL,u)==-1){fprintf(stderr, "Problem semctl init value fin processus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}


  /*permettras d'attendre que le bus parte.*/
  u.val=1;
  if(semctl(semid,3,SETVAL,u)==-1){fprintf(stderr, "Problem semctl init value fin processus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  
  /*attendras que bus mette u.val à 0 pour arreter ce processus & clean la semaphore*/
  u.val=1;
  if(semctl(semid,4,SETVAL,u)==-1){fprintf(stderr, "Problem semctl init value fin processus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  else{
    printf("semaphore created, go nuts kids!\n");
  }

  /*On attend sur la semaphore #1 que bus change sa valeur.*/
  w.sem_num=4;
  w.sem_op=0;
  w.sem_flg=0;
  if(semop(semid,&w,1)==-1){fprintf(stderr, "Problem wait sem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  if(semctl(semid,0,IPC_RMID)==-1){fprintf(stderr, "Problem delete sem : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  printf("finis de travailler, j'ai détruit la semaphore et je m'arrète\n");
  
  return 0;
}
