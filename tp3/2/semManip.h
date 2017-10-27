#ifndef SEM_MANIP_EX_2
#define SEM_MANIP_EX_2

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define FILE_KEY "semManip.c"
#define INT_KEY 5

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short  *array;
  struct seminfo  *__buf;
};


key_t getKey();

int createSem(int nbSem,int initialValue);
int getSemId();
int deleteSem();
int attachMeToSem();

int waitSem(int numSem);

#endif
