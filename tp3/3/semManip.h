/* Not everything is useful here, just did some stuff for other exercises */

#ifndef SEM_MANIP_EX_2
#define SEM_MANIP_EX_2

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define FILE_KEY "semManip.h"
#define INT_KEY 5
#define NB_AREA 10
/*+1 will be the wait semaphore*/
#define NB_SEM NB_AREA+1
/*Not mandatory, i did it because typical process will fork, and therefore init & TP needs to have the same number of process.
If it really needs the two programs real needs to be apart, we can use a message queue or a shared memory, pipe [...]*/
#define NB_PROCESS 10

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};

int createSem();
int getSemId();
int deleteSem();
void printSem();

int addSem(int numSem,int value);
int subSem(int numSem,int value);
int decSem_nowait(int numSem);
int decSem(int numSem);
int incSem(int numSem);
int waitSem(int numSem);
/*Shouldn't ever be used outside of semManip.c*/
key_t getKey();
int createSemAux(int initialValue);

#endif
