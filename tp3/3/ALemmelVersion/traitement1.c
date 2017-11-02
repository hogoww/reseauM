#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct ipc{
	int zone[5];

};

struct sembuf opp;
struct sembuf opv;
int idsem;

struct ipc* init(struct ipc* mem)
{	//crée la clef
	key_t clef = ftok("./generateur.txt", 25);
	key_t clefsem = ftok("./generateur.txt", 12);
	idsem = semget(clefsem, 5, 0666);
	opp.sem_op = -1;
	opp.sem_flg = 0;
	
	opv.sem_op = 2;
	opv.sem_flg = 0;
	
	//recupère l'id de la memoire partagée
	int idMem = shmget (clef,(size_t)0,0666);
	//recupère la memoire partagée et la met dans la structure
	mem=(struct ipc *) shmat(idMem,NULL,0);

	
	if((void*)mem == (void*)-1){
		
		perror("shmat");
		exit(-1);
	}
	
	
	
}
void traitement(struct ipc* mem)
{
	for (int i = 0; i < 5; i++){
		opp.sem_num = i;
		opv.sem_num = i;
		printf("debut du traitement par prog 1\n");
		semop(idsem,&opp,1);
		mem[0].zone[i] += 2;
		mem[0].zone[i] *= 12;
		printf("valeur de la zone : %d \n", mem[0].zone[i]);
		sleep(2);
		printf("liberation de la zone \n");
		semop(idsem,&opv,1);
		sleep(2);

		
	}
	
	
	

}

void main()
{
	struct ipc* memoire[1];
	*memoire = init(*memoire);
	traitement(*memoire);
}
	
	
	