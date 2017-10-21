/*gcc -Wall -o d decomp.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_TREATMENT 5
#define BUFF_SIZE 9


void MainFunc(int nbProcess);
void ForkedFunc(int nbProcess);

struct listIndex{
  int value;
  struct listIndex* next;
};

void listIndex_init(struct listIndex* l){
  l=malloc(sizeof(struct listIndex));
  l->value=-1;
  l->next=NULL;
}

void listIndex_destroy(struct listIndex* l){
  if(l->next==NULL){
    free(l);
  }
  else{
    listIndex_destroy(l->next);
    free(l);
  }
}
				
int listIndex_rm_value(struct listIndex* l,int a){
  if(a<0 || l==NULL || l->value==-1/*first elem of listIndex*/){
    return 1;
  }
  while(l->next!=NULL && l->next->value!=a){
    l=l->next;
  }
  
  struct listIndex* t=l->next;
  l->next=l->next->next;
  free(t);
  return 0;
}

int listIndex_add_value(struct listIndex *l,int a){
  if(a<0){
    return 1;//We only want indexes
  }
  if(l!=NULL){
    while(l->next!=NULL){
      if(l->value==a){
	return 1;//value exist, fails.
      }
      if(l->next->value>a){
	l=l->next;
      }
      else{
	struct listIndex* nl=malloc(sizeof(struct listIndex));
	nl->value=a;
	nl->next=l->next;
	l->next=nl;
	break;
      }
    }
  }
  return 0;
}

struct exitmsg{
  long label;
};

void exitmsg_init(struct exitmsg *m,long numproc){
  m->label=numproc;
}

struct numberMsg{
  int baseNumber;//Will be used as a label back
  int newNumber;
};

struct msg{
  long label;
  struct numberMsg n;
};

void msg_init(struct msg* m,int numNumber,int baseNum){
  m->label=numNumber;
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

struct msg msg_copy(struct msg* m){
  struct msg n;
  n.label=m->label;
  n.n.baseNumber=m->n.baseNumber;
  n.n.newNumber=m->n.newNumber;
  return n;
}


int main(int argc,char** argv){
  if(argc!=2){
    fprintf(stderr,"\n\nUsage : ./decomp NB_PROCESSUS\n\n");//undefined behavior on anything different from an integer
  }
  
  int nbProc=atoi(argv[1]);

  pid_t* children=malloc(nbProc*sizeof(pid_t));
  
  pid_t mainPID=getpid();
  size_t i;
  for(i=0;i<nbProc;i++){
    children[i] = fork ();
    if(children[i]==0){//I'm in the a son.
      free(children);//Don't need them here.
      ForkedFunc(nbProc);
      break;
    }
  }
  
  if(getpid()==mainPID){
    MainFunc(nbProc);
  }
  
  for(i=0;i<nbProc;i++){//We wait for the childs to cleanly stop the process.
    wait(&children[i]);
  }
  
  return 0;  
}


void MainFunc(int nbProcess){
  
  int working=1;
  char* input=malloc(sizeof(char)*BUFF_SIZE);
  int number;
  int mul[MAX_TREATMENT];
  struct listIndex l;
  listIndex_init(l);
  
  while(working){
    scanf("%9s",input);
    if(input[0]=='e' && input[1]=='n' && input[2]=='d'){
      working=0;
      continue;
    }
    number=atoi(input);//too lazy to check if it's an int or not
    if(number<0){
      fprintf(stderr,"I'm only working on positive numbers");
      continue;
    }
    
    
  }
}
