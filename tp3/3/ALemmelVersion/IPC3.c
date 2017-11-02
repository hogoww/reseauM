#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>

struct ipc{
	int zone[5];

};
struct ipc image;

union semun {
	int val;
	struct semid_ds *buf;
	unsigned  short *array;
	struct seminfo * __buf;
};
int sem;
	
int init(key_t clef, key_t clefsem)
{
	if(shmget(clef,(size_t)sizeof(struct ipc),IPC_CREAT|0666)==-1)
	{
		return -1;
	}
	struct ipc * tampon;
	int idMem = shmget (clef,(size_t)0,0666);
	tampon =(struct ipc *) shmat(idMem,NULL,0);
	sem = semget(clefsem,5,IPC_CREAT|0666);
	union semun param;
	param.val = 1;
	
	for(int i = 0; i< 5; i++){
		semctl(sem, i, SETVAL,param );
	}
	if(sem ==-1)
	{
		return -1;
	}
	tampon[0] = image;
	return 0;
}

int supp(int ide)
{

	int destru = shmctl(ide,IPC_RMID,NULL);
	return semctl(sem,0,IPC_RMID);
	
		
	
	
}
int main()
{
	key_t clefsem = ftok("./generateur.txt", 12);
	key_t clef = ftok("./generateur.txt", 25);
	
	for(int i = 0; i < 5; i++){
		image.zone[i] = i+1 *3;
	}

	int ide;
	ide=init(clef, clefsem);
	printf("id semaphore %d \n",sem);
	if(ide==-1)
	{
		return -1;
	}
	printf("id semaphore %d \n",semget(clefsem,5,0666));
	printf("appuyer sur une touche ");
	scanf("%d");
	if(supp(ide)==-1)
	{
		perror("ERREUR DESTROY: ");
		return -1;
	}
	
}