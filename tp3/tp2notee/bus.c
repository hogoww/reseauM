#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define FILE_KEY "init.c"
#define INT_KEY 5

int semid;/*pour une exit propre, même si c'est pas demandé*/


int commencer_tournee(int semid);
int fin_tournee(int semid);
int bus_vide(int semid,int nbplaces);
int ouvre_bus(int semid,int nbplaces);
int empecher_les_gens_de_monter(int semid);
int attente_plus_de_place(int semid);
int laisser_monter_les_gens(int semid);
int laisser_descendre_les_gens(int semid);
int empecher_les_gens_de_descendre(int semid);

int tournee(int semid,int nbplaces){

  while(1){
    printf("\n\n");
    
    if(bus_vide(semid,nbplaces)){fprintf(stderr, "Problem bus_vide : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    
    printf("je suis vide, je laisse des gens monter\n");
    
    if(ouvre_bus(semid,nbplaces)){fprintf(stderr, "Problem ouvre bus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    
    if(laisser_monter_les_gens(semid)==-1){fprintf(stderr, "Problem laisser monter des gens : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    if(attente_plus_de_place(semid)==-1){fprintf(stderr, "Problem attente plus de places : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    printf("je suis pleins, j'empèche les gens de monter\n");

    
    if(empecher_les_gens_de_monter(semid)==-1){fprintf(stderr, "Problem empecher les gens de monter : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    if(empecher_les_gens_de_descendre(semid)==-1){fprintf(stderr, "Problem empecher les gens de monter : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

    if(commencer_tournee(semid)==-1){fprintf(stderr, "Probleme commencer tournée : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    
    printf("Je commence ma tournée!\n");
    sleep(3);
    printf("J'ai finis ma tournée!\n");
    
    if(fin_tournee(semid)==-1){fprintf(stderr, "Probleme commencer tournée : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

    
    if(laisser_descendre_les_gens(semid)==-1){fprintf(stderr, "Problem laisser les gens descendre : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  }
  
}

int commencer_tournee(int semid){
  /*On attends que le bus soit vide*/
  struct sembuf w;
  w.sem_num=3;
  w.sem_op=-1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}


int fin_tournee(int semid){
  /*On attends que le bus soit vide*/
  struct sembuf w;
  w.sem_num=3;
  w.sem_op=1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}

int bus_vide(int semid,int nbplaces){
  /*On attends que le bus soit vide*/
  struct sembuf w;
  w.sem_num=0;
  w.sem_op=-nbplaces;
  w.sem_flg=0;
  return semop(semid,&w,1);
}
  
int ouvre_bus(int semid,int nbplaces){
  /*On attends que le bus soit vide*/
  struct sembuf w;
  w.sem_num=0;
  w.sem_op=nbplaces;
  w.sem_flg=0;
  return semop(semid,&w,1);
}
  
int empecher_les_gens_de_monter(int semid){
  /*Plus de places, on empèche des gens de monter*/
  struct sembuf w;
  w.sem_num=1;
  w.sem_op=1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}


int attente_plus_de_place(int semid){
  /*le bus attends qu'il n'y ai plus de places*/
  struct sembuf w;
  w.sem_num=0;
  w.sem_op=0;
  w.sem_flg=0;
  return semop(semid,&w,1);
}

int laisser_monter_les_gens(int semid){
  /*le bus commence la tournée la tournée, le bus est vide, on autorise des gens à monter*/
  struct sembuf w;
  w.sem_num=1;
  w.sem_op=-1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}


int laisser_descendre_les_gens(int semid){
  /*le bus commence la tournée la tournée, le bus est vide, on autorise des gens à monter*/
  struct sembuf w;
  w.sem_num=2;
  w.sem_op=-1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}

int empecher_les_gens_de_descendre(int semid){
  /*le bus commence la tournée la tournée, le bus est vide, on autorise des gens à monter*/
  struct sembuf w;
  w.sem_num=2;
  w.sem_op=1;
  w.sem_flg=0;
  return semop(semid,&w,1);
}



void sigINT_handler(int signo){
  
  if(signo==SIGINT){
    
    printf("exiting nicely, have a nice day :)\n");
    
    struct sembuf w;
    w.sem_num=4;
    w.sem_op=-1;
    w.sem_flg=0;
    if((semop(semid,&w,1)==-1)){fprintf(stderr, "Problem en rentrant dans le bus : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
    else{
      exit(EXIT_SUCCESS);
    }
  }
}

int main(int argc,char** argv){
  key_t key;
  int nbplaces;
  
  
  if(argc!=2){
    fprintf(stderr,"\n\n Usage : ./init NOMBRE_DE_PASSAGER\n\n");/*Nombre de passager ne seras pas utilisé*/
  }
  
  if(signal(SIGINT,sigINT_handler)==SIG_ERR){fprintf(stderr,"attribution of SIGIN trap failed\n");exit(EXIT_FAILURE);}
  

  nbplaces=atoi(argv[1]);
  
  if((key=ftok(FILE_KEY,INT_KEY))==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));exit(EXIT_FAILURE);}

  /*Deux numero de semaphore, un pour compter les passager, l'autre pour finir ce processus.*/
  if((semid=semget(key,5,0600))==-1){fprintf(stderr, "Problem semget : %s.\n La semaphore est-elle bien créée?\n",strerror(errno));exit(EXIT_FAILURE);}

  tournee(semid,nbplaces);

  return 0;
}
