#ifndef MQ_CALC_H
#define MQ_CALC_H

#define MQ_KEY_FILE "./mqcalc.c"
#define MQ_KEY_NUM 1

#define MQ_C_ADD 0
#define MQ_C_SUB 1
#define MQ_C_MUL 2
#define MQ_C_DIV 3

#include <unistd.h>

struct Query{
  int n1;
  int n2;
  int op;
  int res;
  int cpi;//Calling process ID
};

struct mq_calc{
  long label;
  struct Query q;
};

int mq_calc_init(struct mq_calc *c,int num1,int num2,char operator);
void mq_calc_putReturnLabel(struct mq_calc *c);
void mq_calc_putResult(struct mq_calc *c,int result);


#endif
