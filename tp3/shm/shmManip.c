#include "shmManip.h"

/*
  The implementation of this interface is a bit peculiar, but is a lot easier to use.
  For this to work, the file HAVE to exist, and createShm HAVE to be called first. Won't work otherwise.
  Doesn't handle errors, gives them to the caller.
*/

key_t getKey(){
  static key_t key=-2;/*key=-1 or a positive number, so init will be done once.*/
  if(key==-2)
    key=ftok(FILE_KEY,INT_KEY);
  return key;
  /*If k=-1, trying to ftok again won't work anyway, so you have to check if the file exist, or give another file.*/
}

int createShmInt(){
  return createShmIntAux(1);
}

int getShmIntId(){
  return createShmIntAux(0);/*ShmID is already created, so doesn't need the args again.*/
}


void* attachShm(int attach){
  static int* s=(void*)-1;
  if(attach){
    if(s==(void*)-1){
      s=shmat(getShmIntId(),NULL,0);
    }
    return s;
  }
  else{
    if(s!=(void*)-1){
      shmdt(s);
    }
    else{
      printf("can't detach something that's not attached");
      exit(EXIT_FAILURE);
    }
    return (void*)-1;
  }
}

int deleteShmInt(){
  return shmctl(getShmIntId(),0,IPC_RMID);
}


int createShmIntAux(int creator){
  static int shmId=-2;/*key=-1 or a positive number, so init will be done once.*/
  int returnVal;
   
  if(shmId==-2){
    /*only init will concerne itself with this*/
    if(creator){
      shmId=shmget(getKey(),SIZE_SHM, 0600|IPC_CREAT|IPC_EXCL);
    }
    else{
      shmId=shmget(getKey(),SIZE_SHM, 0600);
    }
  }
  return shmId;
}

