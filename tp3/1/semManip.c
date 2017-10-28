#include "semManip.h"

/*
  The implementation of this interface is a bit peculiar, but is a lot easier to use.
  For this to work, the file HAVE to exist, and createSem HAVE to be called first. Won't work otherwise.
  Doesn't handle errors, gives them to the caller.
*/

key_t getKey(){
  static key_t key=-2;/*key=-1 or a positive number, so init will be done once.*/
  if(key==-2)
    ftok(FILE_KEY,INT_KEY);
  return key;
  /*If k=-1, trying to ftok again won't work anyway, so you have to check if the file exist, or give another file.*/
}

int createSem(int initialValue){
  return createSemAux(initialValue);
}

int createSemAux(int initialValue){
  static int semId=-2;/*key=-1 or a positive number, so init will be done once.*/
  int returnVal;
  union semun u;
   
  if(semId==-2){
    /*only init will concerne itself with this*/
    if(initialValue!=-1){
      semId=semget(getKey(),NB_SEM, 0600|IPC_CREAT);
      if(semId==-1){
	return -1;
      }
      u.val=initialValue;
      returnVal=semctl(semId,0,SETVAL,u);
      printf("init value=%d\n",semctl(getSemId(),0,GETVAL));
      if(returnVal==-1){/*so we can differenciate the two issues*/
	fprintf(stderr, "Problem semctl setVal : %s.\n",strerror(errno));
	return -1;
      }
    }
    else{
      semId=semget(getKey(),NB_SEM, 0600);
      if(semId==-1){
	return -1;
      }
    }
  }
  return semId;
}

int getSemId(){
  return createSem(-1);/*SemID is already created, so doesn't need the args again.*/
}

int deleteSem(){
  return semctl(getSemId(),0,IPC_RMID);
}



/*
We didn't use the SEM_UNDO flag for theses semaphore, because it would cancel the operation processus did in ex2 on exit. Which would make the init bug
It's unimportant in case we only use one operation at a time anyway
*/
int waitSem(int numSem){  
  struct sembuf w;
  w.sem_num=numSem;
  w.sem_op=0;
  w.sem_flg=0;
  return semop(getSemId(),&w,1);
}

int decSem(int numSem){
  struct sembuf dec;
  dec.sem_num=numSem;
  dec.sem_op=-1;
  dec.sem_flg=0;
  return semop(getSemId(),&dec,1);
}

int incSem(int numSem){
  struct sembuf inc;
  inc.sem_num=numSem;
  inc.sem_op=+1;
  inc.sem_flg=0;
  return semop(getSemId(),&inc,1);
}

int addSem(int numSem,int value){
  struct sembuf add;
  add.sem_num=numSem;
  add.sem_op=value;
  add.sem_flg=0;
  return semop(getSemId(),&add,1);  
}

int subSem(int numSem,int value){
  struct sembuf sub;
  sub.sem_num=numSem;
  sub.sem_op=-value;
  sub.sem_flg=0;
  return semop(getSemId(),&sub,1);  
}


int decSem_nowait(int numSem){
  struct sembuf dec;
  dec.sem_num=numSem;
  dec.sem_op=-1;
  dec.sem_flg=IPC_NOWAIT;
  return semop(getSemId(),&dec,1);
}
