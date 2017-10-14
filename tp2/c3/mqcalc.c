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
  c->label=c->q.op;//Each calculator only check one.
  c->q.cpi=getpid()+OFFSET_Q;//so each query find it's owner back
  return 0;
}

void mq_calc_putReturnLabel(struct mq_calc *c){
  c->label=c->q.cpi;
}

void mq_calc_putResult(struct mq_calc *c,int result){
  c->q.res=result;
}

long mq_calc_getMyLabel(struct mq_calc *c){
  return (long) c->q.cpi;
}
