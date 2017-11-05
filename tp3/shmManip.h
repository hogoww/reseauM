#ifndef SHM_MANIP_EXAMPLE
#define SHM_MANIP_EXAMPLE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>

#define FILE_KEY "shmManip.h"
#define INT_KEY 5

/*We'll create an array of 10 integer*/
#define SIZE_SHM 10



int createShmInt();
int getShmId();
int deleteShmId();

key_t getKey();
int createShmIntAux(int creator);

#endif
