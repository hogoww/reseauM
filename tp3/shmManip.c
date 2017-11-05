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

int createShm(){
  return createShmAux(1);
}

int getShmId(){
  return createShmAux(0);/*ShmID is already created, so doesn't need the args again.*/
}


int createShmAux(int creator){
  static int shmId=-2;/*key=-1 or a positive number, so init will be done once.*/
  int returnVal;
  union shmun u;
   
  if(shmId==-2){
    /*only init will concerne itself with this*/
    if(initialValue!=-1){
      shmId=shmget(getKey(),NB_SHM, 0600|IPC_CREAT);
      if(shmId==-1){
	return -1;
      }
      u.val=initialValue;
      returnVal=shmctl(shmId,0,SETVAL,u);
      u.val=0;/*accumulator to check when we're done*/
      returnVal=shmctl(shmId,1,SETVAL,u);
      printf("init value=%d\n",shmctl(getShmId(),0,GETVAL));
      if(returnVal==-1){/*so we can differenciate the two issues*/
	fprintf(stderr, "Problem shmctl setVal : %s.\n",strerror(errno));
	return -1;
      }
    }
    else{
      shmId=shmget(getKey(),NB_SHM, 0600);
      if(shmId==-1){
	return -1;
      }
    }
  }
  return shmId;
}

int deleteShm(){
  return shmctl(getShmId(),0,IPC_RMID);
}
