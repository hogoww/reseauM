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
  return key;/*If k=-1, trying to ftok again won't work anyway, so you have to check if the file exist, or give another file.*/
}

int createSem(int nbSem,int initialValue){
  static int semId=-2;/*key=-1 or a positive number, so init will be done once.*/
  int returnVal;
  union semun u;
   
  if(semId==-2){
    semId=semget(getKey(),nbSem, 0600|IPC_CREAT);
    if(semId==-1){
      return -1;
    }
    u.val=initialValue;
    returnVal=semctl(semId,0,SETVAL,u);
    if(returnVal==-1){/*so we can differenciate the two issues*/
      fprintf(stderr, "Problem semctl setVal : %s.\n",strerror(errno));
      return -1;
    }
  }
  return semId;
}

int getSemId(){
  return createSem(0,0);/*SemID is already created, so doesn't need the args again.*/
}

int deleteSem(){
  return semctl(createSem(0,0),0,IPC_RMID);
}

int attachMeToSem(){
  return createSem(0,0);
}

int waitSem(int numSem){
  struct sembuf dec;
  struct sembuf w;
  dec.sem_num=numSem;
  dec.sem_op=-1;
  dec.sem_flg=SEM_UNDO;
  if(semop(getSemId(),&dec,1)==-1){fprintf(stderr, "Problem semop dec : %s.\n",strerror(errno));return -1;}

  w.sem_num=numSem;
  w.sem_op=0;
  w.sem_flg=SEM_UNDO;
  return semop(getSemId(),&w,1);
}
