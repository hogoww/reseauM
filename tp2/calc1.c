#include <stdio.h>
#include <sys/msg.h>
#include <sys/msg.h>
#include <string.h>


int queryEval(char s[],int size){
  int res,v1,v2;
  
  v1=(int)s[1]-48;
  v2=(int)s[2]-48;

  switch(s[0]){
  case '+':
    res=v1+v2;
    break;
  case '-':
    res=v1+v2;
    break;
  case '*':
    res=v1+v2;
    break;
  case '/':
    res=v1+v2;
    break;
  default:
    fprintf(stderr,"Query %s couldn't be evaluated\n",s);
    res=0;
  }
  return res;
}

int main(){
  
  key_t k=ftok("/readme.dms",3147);
  
  int f_id=msgget(k,IPC_CREAT|0666);
  

  return 0;
}
