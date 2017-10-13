#include "mqcalc.h"


int mq_calc_init(struct mq_calc *c,int num1,int num2,char operator){
  c->q.n1=num1;
  c->q.n2=num2;
  switch(operator){
  case '+':
    c->q.op=MQ_C_ADD;
    break;
  case '-':
    c->q.op=MQ_C_SUB;
    break;
  case '*':
    c->q.op=MQ_C_MUL;
    break;
  case '/':
    c->q.op=MQ_C_DIV;
    break;
  default:
    return 1;
  }
  c->label=0;//to be changed
  c->q.cpi=1;
  return 0;
}

void mq_calc_putReturnLabel(struct mq_calc *c){
  c->label=c->q.cpi;
}

void mq_calc_putResult(struct mq_calc *c,int result){
  c->q.res=result;
}
