#ifndef __List__Assoc__H__
#define __List__Assoc__H__

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct list{
  char* v;
  struct list* next;
};


struct list* make_list(char* val);
void delete_list(struct list* l);/*doesn't free what's in v*/
void delete_list_and_values(struct list* l);
struct list* add_value_list(struct list *l,char* v);
int size_list(struct list*l);
void DisplayList(struct list* l);
struct list* getIndex_list(struct list* l,int index);


struct listAssoc{
  char* k;
  struct list* l;
  struct listAssoc* next;
};

struct listAssoc* make_ListAssoc(char* key);
void delete_listAssoc_and_key_and_values(struct listAssoc* l);
struct listAssoc* get_key_listAssoc(struct listAssoc* l,char *key);
struct listAssoc* addValue_to_key_list(struct listAssoc* list,char* key,char* value);
int size_listAssoc(struct listAssoc*l);
void DisplayListAssoc(struct listAssoc* list);
struct listAssoc* getIndex_listAssoc(struct listAssoc* l,int index);
struct listAssoc* destroyAndChangeList_listAssoc(struct listAssoc* l,char* key,struct list * li);
void delNode_listAssoc(struct listAssoc* l);
struct listAssoc* removeThatKey_listAssoc(struct listAssoc* l,char *key);

#endif
