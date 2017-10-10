/* gcc -pedantic -Wall -o m tpnote.c -std=c99 -lpthread */

#include <stdio.h>//printf
#include <unistd.h>//sleep
#include <stdlib.h>//free
#include <pthread.h>//mutex & cond var

//problématique mutex pour un certain nombre de thread
struct n_verrou{
  int nb_lock;//compteur
  int max_lock;//nombre de thread maximum
  pthread_mutex_t mut;//Le verrou n'est utilisé que pour modifier cette struture.
  pthread_cond_t cond;//Annonce qu'il y a de la place dans notre mutex
};

int n_verrou_init(struct n_verrou* v,int k){
  v->max_lock=k;
  v->nb_lock=0;
  if((pthread_mutex_init(&(v->mut),NULL))!=0){
    fprintf(stderr,"erreur lors de la création du verrou multiple dans n_verrou_init\n");
    return -1;
  }//else le verrou est initialisé

  if((pthread_cond_init(&(v->cond),NULL))!=0){
    fprintf(stderr,"erreur lors de la création de la variable conditionnelle dans n_verrou_init\n");
    return -1;
  }//else la cond est bien initialisée
  return 0;//Tout s'est bien passé.
}

int n_verrou_lock(struct n_verrou* v){
  //On prend le mutex pour être sûr que la structure n'est touchée que par nous 
  if(pthread_mutex_lock(&(v->mut))!=0){
    fprintf(stderr,"erreur lors du lock du mutex dans n_verrou_lock\n");
    return -1;
  }
  //Si il y a trop de monde, on boucle en attendant qu'il y ai de la place, annoncé par la cond
  while(v->nb_lock>=v->max_lock){
    pthread_cond_wait(&(v->cond),&(v->mut));
  }
  // ZONE CRITIQUE
  //Il y a de la place, on reserve celle du processus qui a appellé la fonction

  (v->nb_lock)++;
  //FIN ZONE CRITIQUE
  if(pthread_mutex_unlock(&(v->mut))!=0){
    fprintf(stderr,"erreur lors du unlock du mutex dans n_verrou_lock\n");
    return -1;
  }

  return 0;//Tout s'est bien passé.
}


int n_verrou_unlock(struct n_verrou* v){
  if(pthread_mutex_lock(&(v->mut))!=0){
    fprintf(stderr,"erreur lors du lock du mutex dans n_verrou_lock\n");
    return -1;
  }
  //ZONE CRITIQUE
  if(v->nb_lock<=0){
    fprintf(stderr,"Ce mutex n'est utilisé par personne, pas besoin de l'unlock !\n");
    return -1;
  }
  else{

    (v->nb_lock)--;
  }
  //FIN ZONE CRITIQUE
  //On broadcast que le mutex est libre avant de le rendre
  if(pthread_cond_broadcast(&(v->cond))!=0){
    fprintf(stderr,"erreur lors du broadcast de cond dans n_verrou_unlock\n");
    return -1;
  }


  if(pthread_mutex_unlock(&(v->mut))!=0){
    fprintf(stderr,"erreur lors du lock du mutex dans n_verrou_unlock\n");
    return -1;
  }
  return 0;//Tout s'est bien passé.
}

int n_verrou_destroy(struct n_verrou* v){//Ne dispence pas d'un free de la structure!

  if(v->nb_lock>0){
    fprintf(stderr,"Je refuse cordialement de détruire le verrou, des processus l'utilisent encore\n");
    return -1;
  }
  
  if((pthread_mutex_destroy(&(v->mut)))!=0){
    fprintf(stderr,"erreur lors de la destruction du verrou multiple dans n_verrou_destroy\n");
    return -1;
  }//else le verrou est détruit

  if((pthread_cond_destroy(&(v->cond)))!=0){
    fprintf(stderr,"erreur lors de la destruction de la variable conditionnelle dans n_verrou_destroy\n");
    return -1;
  }//else la cond est bien détruit.
  return 0;
}


void traitement1(){
  sleep(1);
}

void traitement2(){
  sleep(1);
}

void traitement3(){
  sleep(2);
}


void* threadWork(void* param){
  struct n_verrou* p=(struct n_verrou*)param;

  printf("traitement 1 commencé\n");
  traitement1();
  printf("traitement 1 finis\n");
  if(n_verrou_lock(p)!=0){
    fprintf(stderr,"problème de lock du mutex multiple dans threadWork\n");
  }

  printf("traitement 2 commencé\n");
  traitement2();
  printf("traitement 2 finis\n");
  if(n_verrou_unlock(p)!=0){
    fprintf(stderr,"problème d'unlock du mutex multiple dans threadWork\n");
    pthread_exit(NULL);
  }
  printf("traitement 3 commencé\n");
  traitement3();
  printf("traitement 3 finis\n");

  pthread_exit(NULL);
}




int main(int argc, char** argv){
  int nbT;
  int nbMT;
  if(argc!=3){
    fprintf(stderr,"usage: ./m NB_THREAD NB_MAX_THREAD_SUR_TRAITEMENT2\n");
    return -1;
  }
  else{
    nbT=atoi(argv[1]);
    nbMT=atoi(argv[2]);
  }

  struct n_verrou* p=malloc(sizeof(struct n_verrou));//Pas besoin de plus que cette structure, comme nous ne faisons pas de traitement

  if(n_verrou_init(p,nbMT)!=0){//on init
    fprintf(stderr,"problème init n_verrou dans le main");
  }
  
  pthread_t* idT=malloc(sizeof(pthread_t)*nbT);
  for(size_t i=0;i<nbT;++i){//On créé tout les threads
    if(pthread_create(&idT[i],NULL,threadWork,(void*)p)){
      fprintf(stderr,"failed creating a thread\n");
      return -1;
    }
  }

  for(size_t i=0;i<nbT;++i){//On les attends
    pthread_join(idT[i],NULL);
  }

  if(n_verrou_destroy(p)!=0){//on détruit le verrou
    fprintf(stderr,"problème destroy n_verrou dans le main");
  }
  
  free(p);//on rend la mémoire allouée.
  free(idT);
  
  return 0;
}
