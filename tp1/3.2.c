/*gcc -ansi -Wall -pedantic -o prog 3.2.c -lpthread -std=c99*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
    int* nbWorkers;
    int* nbWaitingWorkers;
    int beginWork;
    int nbTaches;
    int tacheCourrante;

    pthread_cond_t beginWorkCond;
    pthread_cond_t controlePoint;
    pthread_mutex_t mut;
    pthread_mutex_t mutBeginWork;
} threadParameter;

void initThreadParameter(threadParameter* p, int nbTaches){
  p->nbWorkers=malloc(sizeof(int*));
  *(p->nbWorkers)=0;
  p->nbWaitingWorkers=malloc(sizeof(int*));
  *(p->nbWaitingWorkers)=0;
  p->beginWork=0;
  p->nbTaches = nbTaches;
  p->tacheCourrante = 0;

  pthread_cond_init(&(p->beginWorkCond),NULL);
  pthread_cond_init(&(p->controlePoint),NULL);
  pthread_mutex_init(&(p->mut),NULL);
  pthread_mutex_init(&(p->mutBeginWork),NULL);
}

void destroyThreadParameter(threadParameter* p){
  free(p->nbWorkers);
  free(p->nbWaitingWorkers);
}

void* f(void * p)
{
  threadParameter* c=(threadParameter*)p;
  int myself = pthread_self()  % 100;

    printf("[worker-%i] Je suis prêt, en attente de tout le monde\n", myself);
    pthread_mutex_lock(&(c->mutBeginWork));
    while(!c->beginWork)
    {
        pthread_cond_wait(&(c->beginWorkCond),&(c->mutBeginWork));
    }
    pthread_mutex_unlock(&(c->mutBeginWork));

  int min = 1, max = 5, r;
  while (c->tacheCourrante != c->nbTaches)
  {
        r = (rand() % (max - min)) + min;
        printf("[worker-%i] Je commence le travail de la tâche %i, temps nécessaire : %is\n", myself, c->tacheCourrante + 1, r);
        sleep(r); //working on stuff

        pthread_mutex_lock(&(c->mut));
        *(c->nbWaitingWorkers) += 1;
        if(*(c->nbWaitingWorkers) == *(c->nbWorkers))
        {
            *(c->nbWaitingWorkers) = 0;
            printf("[worker-%i] J'ai fini de travailler, j'étais le dernier donc je préviens les autres\n", myself);
            pthread_cond_broadcast(&(c->controlePoint));
            printf("Tout le monde a terminé la %ie tâche\n\n", c->tacheCourrante + 1);
            c->tacheCourrante += 1;
        }
        else
        {
            printf("[worker-%i] J'ai fini de travailler, j'attends les autres avant continuer\n", myself);
            pthread_cond_wait(&(c->controlePoint),&(c->mut));
        }
        pthread_mutex_unlock(&(c->mut));

    }



  pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));

    int nTasks = -1, ok = 0;
    printf("Saisir nombre de tâches : ");
    while(!ok)
    {
        if(scanf("%i", &nTasks) == 1 && nTasks > 0)
        {
            ok = 1;
        }
        else
        {
            char c;
            while( (c = getchar()) != EOF && c != '\n') { } // Pour vider stdin
            printf("Saisir un nombre entier positif : ");
        }
    }

    int nWorkers = -1;
    ok = 0;
    printf("Saisir nombre de travailleurs : ");
    while(!ok)
    {
        if(scanf("%i", &nWorkers) == 1 && nWorkers > 0)
        {
            ok = 1;
        }
        else
        {
            char c;
            while( (c = getchar()) != EOF && c != '\n') { } // Pour vider stdin
            printf("Saisir un nombre entier positif : ");
        }
    }

    threadParameter* p=malloc(sizeof(threadParameter));
    initThreadParameter(p, nTasks);

    pthread_t idT[nWorkers];
    for(size_t i=0; i < nWorkers; i++)
    {
        pthread_mutex_lock(&(p->mut));
        if(pthread_create(&idT[i],NULL,f,(void*)p))
        {
            fprintf(stderr,"failed creating a thread\n");
            return 1;
        }
        *(p->nbWorkers) += 1;
        pthread_mutex_unlock(&(p->mut));
    }

    // On dit à tous les travailleurs de commencer à bosser
    p->beginWork = 1;
    pthread_cond_broadcast(&(p->beginWorkCond));

    for(size_t i = 0; i < nWorkers; i++)
    {
      pthread_join(idT[i],NULL);
    }

    destroyThreadParameter(p);
    free(p);
    return 0;
}
