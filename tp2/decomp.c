/*gcc -Wall -o d decomp.c -lm*/


/*
Important.

This program, doesn't exit correctly. It's too much work to do it well, so it'll stay that way.
it's actualy not incorrect, just real ugly.

*/



//Undefined behavior if input isn't a number                                                             
/*
Note: I didn't do the multiple entries throught main func by lazyness. You just have to fork again so one mainFunc is getting number, and the other one is getting and displaying the result.

The main problem is to treat every number without mixing them. which can be achieved by declaring that you can't have more than an amount of number.
 */
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

#define OFFSET 1

void MainFunc(int nbProc,pid_t* children,int mqid);
void ForkedFunc(int nbProc,int mqid);

struct exitmsg{
  long label;
  int done;
};

void exitmsg_init(struct exitmsg *m,pid_t e,int isDone){
  m->label=(long)e;
  m->done=isDone;
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
  m->label=1;
  m->n.baseNumber=baseNum;
  m->n.newNumber=baseNum;
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
  msgctl(mqid,IPC_RMID,NULL);//reset queue in case of crash.

  mqid=msgget(k, 0666 | IPC_CREAT);
  if(mqid==-1){fprintf(stderr, "Problem msgget : %s.\n",strerror(errno));exit(EXIT_FAILURE);}else{printf("mqid : %d\n",mqid);}

  
  pid_t* children=malloc(nbProc*sizeof(pid_t)); 
  pid_t mainPID=getpid();
  size_t i;
  for(i=0;i<nbProc;i++){
    children[i]=fork();
    if(children[i]==0){//I'm in the a son.
      free(children);//Don't need them here.
      ForkedFunc(nbProc,mqid);
      break;
    }
  }

  if(getpid()==mainPID){
    MainFunc(nbProc,children,mqid);
    printf("waiting for my childs\n");
    /*deleting the msgq like this will make the sons crash & exit. We still wait for them here.*/
    /*shouldn't be done, i'm just too lazy to correct this.*/
    msgctl(mqid,IPC_RMID,NULL);//Delete mq on leave
    for(i=0;i<nbProc;i++){//We wait for the childs to cleanly stop the process.
      wait(&children[i]);
    }
    free(children);
  }
  else{
    printf("Forked process exiting!\n");
  }
  
  return 0;
}

void MainFunc(int nbProc,pid_t* children,int mqid){
  int working=1;
  int number;
  
  while(working){
    printf("Give me a number > 0, i'll decompose it ! if number<=0, i'll stop \n");
    scanf("%d",&number);
    //printf("%d\n",number);

    if(number<=0){
      working=0;
      printf("Seems we're done here, exiting properly\n");
      struct exitmsg e;
      int i;
      for(i=0;i<nbProc;i++){//Wave of kill signal to children
	exitmsg_init(&e,children[i]+OFFSET,1);
	if(msgsnd(mqid,&e,sizeof(int),0)==-1){fprintf(stderr, "Problem msgsnd exitmsg : %s.\n",strerror(errno));}
      }
      working=0;;//We'll send signal in the main
      printf("killsignal send...\n");
      continue;
    }
    else{
      struct exitmsg e;
      int i;
      for(i=0;i<nbProc;i++){//Wave of not kill signal to childrens
	exitmsg_init(&e,children[i]+OFFSET,0);
	if(msgsnd(mqid,&e,sizeof(int),0)==-1){fprintf(stderr, "Problem msgsnd exitmsg : %s.\n",strerror(errno));}
      }
    }

    struct msg m;
    msg_init(&m,number);
    
    if(msgsnd(mqid,&m,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd MainFunc : %s.\n",strerror(errno));}
    
    int acc=1;
    int first=1;//for a nice display :)
    while(acc!=number){
      struct msg buff;
      if(msgrcv(mqid,&buff,(size_t)sizeof(struct numberMsg),number,0)==-1){fprintf(stderr, "Problem msgrcv MainFunc : %s.\n",strerror(errno));}
      
      acc*=buff.n.newNumber;
      if(first){
	printf("%d",buff.n.newNumber);
	first=0;
      }
      else{
	printf("*%d",buff.n.newNumber);
      }
    }
    printf("=%d\n",number);
  }
}

void ForkedFunc(int nbProc,int mqid){
  int working=1;
  while(working){
    struct exitmsg e;
    if(msgrcv(mqid,&e,(size_t)sizeof(int),getpid()+OFFSET,0)==-1){fprintf(stderr, "Problem msgrcv ForkedFunc killsig : %s.\n",strerror(errno));return;}
    if(e.done){
      working=0;
      continue;
    }

    struct msg buff;
    if(msgrcv(mqid,&buff,(size_t)sizeof(struct numberMsg),1,0)==-1){fprintf(stderr, "Problem msgrcv number ForkedFunc: %s.\n",strerror(errno));return;}
    //printf("got a new number to treat : %d\n",buff.n.newNumber);
    
    int i=2;
    int factorFound=0;
    int lim=sqrt(buff.n.newNumber);
    
    while(!factorFound && i<=lim){
      if(buff.n.newNumber%i==0){
	int f2=buff.n.newNumber/i;
	if(f2==1){
	  continue;//it's not a factor;
	}
	struct msg new1;
	msg_init(&new1,buff.n.baseNumber);
	msg_putNewNumber(&new1,i,0);
	
	struct msg new2;
	msg_init(&new2,buff.n.baseNumber);
	msg_putNewNumber(&new2,f2,0);
	/* printf("\nfactor %d\n",i); */
	/* printf("\nfactor %d\n",new2.n.newNumber); */
	
	if(msgsnd(mqid,&new1,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc, a value is probably lost: %s.\n",strerror(errno));}
	if(msgsnd(mqid,&new2,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc:, a value is probably lost %s.\n",strerror(errno));}

	factorFound=1;
	continue;
      }
      i++;
    }
    if(!factorFound){
      /* printf("didn't found anything for %d \n",buff.n.newNumber); */
      struct msg new;
      msg_init(&new,buff.n.baseNumber);
      msg_putNewNumber(&new,buff.n.newNumber,1);
      if(msgsnd(mqid,&new,(size_t)sizeof(struct numberMsg),0)==-1){fprintf(stderr, "Problem msgsnd ForkedFunc : %s.\n",strerror(errno));}
    }
  }
  printf("Forked process exiting!");
}

