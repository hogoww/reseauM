/*
  This interface is to facilitate the access to only one memory segment.
*/

#ifndef SHM_MANIP_EXAMPLE
#define SHM_MANIP_EXAMPLE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define FILE_KEY "shmManip.h"
#define INT_KEY 5

/*We'll create an array of 10 integer*/
#define SIZE_SHM 10

#define ATTACH 1
#define DETTACH 0

int createShmInt();
int getShmIntId();
void* attachShmInt(int attach);/* Use macro for the arguments, may exit program if you try to detach without ataching first*/
int deleteShmInt();

key_t getKey();
int createShmIntAux(int creator);


#endif
