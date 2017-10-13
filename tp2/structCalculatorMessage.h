#ifndef STRUCT_CALCULATOR_MESSAGE_C
#define STRUCT_CALCULATOR_MESSAGE_C

#define CALC_ADD 0
#define CALC_SUB 1
#define CALC_DIV 2
#define CALC_MUL 3


#define MESSAGE_QUEUE_FILE_CALC "./msgsend.c"
#define MESSAGE_QUEUE_FILE_PROG_ID 1

#include <unistd.h>
#include <stdio.h>

struct Query{
  int n1;
  int n2;
  int op;//Become useless on lastmodel
  int res;
  int cpi;//Calling process ID;
};

struct calculator_Message{
  long int label;
  struct Query q;
};

int calculator_Message_init(struct calculator_Message* cm,int num1,int num2,char operator);
void calculator_Message_switchLabelToReturnLabel(struct calculator_Message* cm);
void calculator_Message_putResult(struct calculator_Message* cm,int result);

#endif
