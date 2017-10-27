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
#define NB_SEM 2

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};


int createSem(int initialValue);
int getSemId();
int deleteSem();

int waiting();

int addSem(int numSem,int value);
int subSem(int numSem,int value);
/* private func*/
int decSem(int numSem);
int incSem(int numSem);
int waitSem(int numSem);

key_t getKey();
int createSemAux(int initialValue);
#endif
