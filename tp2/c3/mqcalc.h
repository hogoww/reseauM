#ifndef MQ_CALC_H
#define MQ_CALC_H

#define MQ_KEY_FILE "./t.t"
#define MQ_KEY_NUM 1

#define MQ_C_ADD 1
#define MQ_C_SUB 2
#define MQ_C_MUL 3
#define MQ_C_DIV 4

#define OFFSET_Q 4//4 operator to treat.

#include <unistd.h>

struct Query{
  int n1;
  int n2;
  int op;
  int res;
  long cpi;//Calling process ID
};

struct mq_calc{
  long label;
  struct Query q;
};

int mq_calc_init(struct mq_calc *c,int num1,int num2,char operator);
void mq_calc_putReturnLabel(struct mq_calc *c);
void mq_calc_putResult(struct mq_calc *c,int result);
long mq_calc_getMyLabel(struct mq_calc *c);

#endif
