#include "structCalculatorMessage.h"

int calculator_Message_init(struct calculator_Message* cm,int num1,int num2,char operator){

  cm->q.n1=num1;
  cm->q.n2=num2;
  switch(operator){
  case '+':
    cm->q.op=CALC_ADD;
    break;
  case '-':
    cm->q.op=CALC_SUB;
    break;
  case '*':
    cm->q.op=CALC_DIV;
    break;
  case '/':
    cm->q.op=CALC_MUL;
    break;
  default:
    fprintf(stderr,"wrong operator\n");
    return 1;
  }  
  cm->label=cm->q.op;//Auto label for last model
  cm->q.op=operator;
  cm->q.res=0;
  //printf("%d\n",getpid());
  cm->q.cpi=1;//getpid()+4;//Auto return adress
  //offset so label 4 in vue of second model.
  return 0;
}

void calculator_Message_switchLabelToReturnLabel(struct calculator_Message* cm){
  cm->label=cm->q.cpi;
}

void calculator_Message_putResult(struct calculator_Message* cm,int result){
  cm->q.res=result;
}
