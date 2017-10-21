/*gcc -Wall -o d decomp.c -lm*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>

#define MAX_TREATMENT 5
#define BUFF_SIZE 9

void MainFunc(int nbProcess,int mqid);
void ForkedFunc(int nbProcess,int mqid);

struct exitmsg{
  long label;
};

void exitmsg_init(struct exitmsg *m,pid_t e){
  m->label=(long)e;
}

struct numberMsg{
  int baseNumber;//Will be used as a label back
  int newNumber;
};

struct msg{
  long label;
  struct numberMsg n;
};

void msg_init(struct msg* m,int baseNum){
  m->label=2;
  m->n.baseNumber=baseNum;
  m->n.newNumber=-1;
}

void msg_putNewNumber(struct msg *m,int newNum,int isAtomic){
  if(isAtomic){
    m->label=m->n.baseNumber;
  }
  m->n.newNumber=newNum;
  //else label stay the same
}


int main(int argc,char** argv){
  if(argc!=2){
    fprintf(stderr,"\n\nUsage : ./decomp NB_PROCESSUS\n\n");//undefined behavior on anything different from an integer
    return 1;
  }
  
  int nbProc=atoi(argv[1]);

  
  key_t k=ftok("d",35);//progname
  if(k==-1){fprintf(stderr, "Problem ftok : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("key : %d\n",k);}
  
  int mqid=-1;
  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){fprintf(stderr, "Problem msgget : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("mqid : %d\n",mqid);}

  
  pid_t* children=malloc(nbProc*sizeof(pid_t)); 
  pid_t mainPID=getpid();
  size_t i;
  for(i=0;i<nbProc;i++){
    children[i] = fork ();
    if(children[i]==0){//I'm in the a son.
      free(children);//Don't need them here.
      ForkedFunc(nbProc,mqid);
      break;
    }
  }
  
  if(getpid()==mainPID){
    MainFunc(nbProc,mqid);
  }
  
  struct exitmsg e;
  for(i=0;i<nbProc;i++){//Wave of kill signal to childs
    if(msgsnd(mqid,&e,0,0)==-1){fprintf(stderr, "Problem msgsnd exitmsg : %s.\n",strerror(errno));}

    exitmsg_init(&e,children[i]);
    
  }
  
  for(i=0;i<nbProc;i++){//We wait for the childs to cleanly stop the process.
    wait(&children[i]);
  }
  
  msgctl(mqid,IPC_RMID,NULL);//Delete mq on leave
  
  return 0;  
}

int getFirstIndex(int* tab,int size){
  int i;
  while(i<size){
    if(tab[i]==-1){
      return i;//this spot is empty
    }
    i++;
  }
  return -1;//tab is full
}

void MainFunc(int nbProcess,int mqid){
  
  int working=1;
  char* input=malloc(sizeof(char)*BUFF_SIZE);
  int number;

  
  while(working){
    printf("Give me a number, i'll decompose it ! Send \"end\" to make me exit\n");
    scanf("%9s",input);
    printf("%s\n",input);

    if(input[0]=='\0'){
      fprintf(stderr,"i don't like empty strings");
      continue;
    }

    if(input[0]=='e' && input[1]=='n' && input[2]=='d'){
      working=0;
      printf("Seems we're done here, exiting properly");
      break;//We'll send signal in the main
    }
    number=atoi(input);//too lazy to check if it's an int or not
    if(number<0){
      fprintf(stderr,"I'm only working on positive numbers");
      continue;
    }

    struct msg m;
    msg_init(&m,number);
    
    if(msgsnd(mqid,&m,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd MainFunc : %s.\n",strerror(errno));}
    
    int acc;
    while(acc!=number){
      struct msg buff;
      if(msgrcv(mqid,&buff,(size_t)sizeof(struct numberMsg),number,0)==-1){fprintf(stderr, "Problem msgrcv MainFunc : %s.\n",strerror(errno));}
      printf("%d ",buff.n.newNumber);
      acc*=buff.n.newNumber;
    }
    printf("= %d",number);
  }
  free(input);
}

void ForkedFunc(int nbProcess,int mqid){
  int working=1;
  while(working){
    struct exitmsg e;
    if(msgrcv(mqid,&e,(size_t)0,getpid(),IPC_NOWAIT)==-1){if(!errno==ENOMSG){fprintf(stderr, "Problem msgrcv : %s.\n",strerror(errno));}}else{working=0;continue;}

    struct msg buff;
    if(msgrcv(mqid,&buff,(size_t)sizeof(struct numberMsg),1,0)==-1){fprintf(stderr, "Problem msgrcv ForkedFunc: %s.\n",strerror(errno));}

    int i=2;
    int factorFound=0;
    int lim=sqrt(buff.n.baseNumber);
    while(!factorFound && i<lim){
      if(buff.n.newNumber%i==0){
	struct msg new1;
	msg_init(&new1,buff.n.baseNumber);
	msg_putNewNumber(&new1,i,0);
	struct msg new2;
	msg_init(&new2,buff.n.baseNumber);
	msg_putNewNumber(&new1,buff.n.baseNumber/i,0);

	if(msgsnd(mqid,&new2,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc: %s.\n",strerror(errno));}
	if(msgsnd(mqid,&new2,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc: %s.\n",strerror(errno));}

	factorFound=1;
	continue;
      }
      i++;
    }
    if(!factorFound){
      struct msg new;
      msg_init(&new,buff.n.baseNumber);
      msg_putNewNumber(&new,buff.n.newNumber,1);
      if(msgsnd(mqid,&new,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc : %s.\n",strerror(errno));}
    }
  }
}

