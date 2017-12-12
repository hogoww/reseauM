#include "listAssoc.h"


struct list* make_list(char* val){
  struct list* res=malloc(sizeof(struct list));
  res->v=val;
  res->next=NULL;
  return res;
}

void delete_list(struct list* l){/*free only "next"s pointers*/
  if(!l){
    return;
  }
  delete_list(l->next);
  free(l);
}

void delete_list_and_values(struct list* l){
  if(l==NULL)
    return;
  if(l->next==NULL)
    delete_list_and_values(l->next);
  free(l->v);
  free(l);
}

struct list* add_value_list(struct list *l,char* v){/*to be able to chain those operations*/
  if(l==NULL){
    return make_list(v);
  }

  struct list* base=l;
  while(l->next!=NULL){
    l=l->next;
  }

  l->next=malloc(sizeof(struct list));
  l->next->v=v;
  l->next->next=NULL;
  
  return base;
}


int size_list_aux(struct list*l,int acc){
  if(l==NULL){
    return acc;
  }
  else{
    return size_list_aux(l->next,acc+1);
  }
}

int size_list(struct list*l){
  return size_list_aux(l,0);
}


void DisplayList_aux(struct list* l,int currentNum){
  if(!l){
    return;
  }
  else{
    printf("\t%d - %s\n",currentNum,l->v);
    DisplayList_aux(l->next,currentNum+1);
  }
}

void DisplayList(struct list* l){
  DisplayList_aux(l,0);
}


struct list* getIndex_list_Aux(struct list* l,int index,int depth){
  if(l==NULL || depth>index){
    return NULL;
  }
  else{
    if(index==depth){
      return l;
    }
    else{
      return getIndex_list_Aux(l,index,depth+1);
    }
  }
}

struct list* getIndex_list(struct list* l,int index){
  return getIndex_list_Aux(l,index,0);
}

/* ASSOCIATIVE LIST PART*/


struct listAssoc* make_ListAssoc(char* key){
  struct listAssoc* res=malloc(sizeof(struct listAssoc));
  res->k=key;
  res->l=NULL;
  res->next=NULL;
  return res;
}

void delete_listAssoc(struct listAssoc* l){/*free only "next"s pointers*/
  if(l->next!=NULL)
    delete_listAssoc(l->next);
  if(l->l==NULL){
    delete_list(l->l);
  }
  free(l);
}

void delete_listAssoc_and_key_and_values(struct listAssoc* l){
  if(l==NULL){
    return;
  }
  delete_listAssoc_and_key_and_values(l->next);
  delete_list_and_values(l->l);
  free(l->k);
  free(l);
}

struct listAssoc* get_key_listAssoc(struct listAssoc* l,char *key){
  if(l==NULL){
    return NULL;
  }
  else{
    if(!strcmp(key,l->k))
       return l;
    else{
      if(l->next==NULL){
	l->next=make_ListAssoc(key);
	return l->next;
      }
      else{
	return get_key_listAssoc(l->next,key);
      }
    }
  }
}

struct listAssoc* addValue_to_key_list(struct listAssoc* list,char* key,char* value){
  struct listAssoc* l=get_key_listAssoc(list,key);
  if(l->l==NULL){
    l->l=make_list(value);
  }
  else{
    add_value_list(l->l,value);
  }
  return list;
}


int size_listAssoc_aux(struct listAssoc*l,int acc){
  if(l==NULL){
    return acc;
  }
  else{
    return size_listAssoc_aux(l->next,acc+1);
  }
}


int size_listAssoc(struct listAssoc*l){
  return size_listAssoc_aux(l,0);
}

void DisplayListAssoc_Aux(struct listAssoc* list,int currentNum){
  //printf("Displaying listAssoc\n");
  if(list==NULL){
    //printf("empty\n");
    return;
  }
  else{
    if(list->k==NULL){
      printf("isn't supposed to happend\n");
    }
    else{
      printf("%d - %s\n",currentNum,list->k);
    }
    DisplayList(list->l);
    DisplayListAssoc_Aux(list->next,currentNum+1);
  }
}

void DisplayListAssoc(struct listAssoc* l){
  DisplayListAssoc_Aux(l,0);
}


struct listAssoc* getIndex_listAssoc_Aux(struct listAssoc* l,int index,int depth){
  if(l==NULL || depth>index){/*DEVRAS RENVOYER NULL POUR CONTINUER LE TRAITEMENT*/
    return NULL;
  }
  else{
    if(index==depth){
      return l;
    }
    else{
      return getIndex_listAssoc_Aux(l,index,depth+1);
    }
  }
}

struct listAssoc* getIndex_listAssoc(struct listAssoc* l,int index){
  return getIndex_listAssoc_Aux(l,index,0);
}


struct listAssoc* destroyAndChangeList_listAssoc(struct listAssoc* l,char* key,struct list * li){
  struct listAssoc* t;
  if(l==NULL){
    t=l=make_ListAssoc(key);
  }
  else{
    t=get_key_listAssoc(l,key);
  }
  /* printf("%s\n",key); */
  /* DisplayList(li); */
  if(t->l){
    delete_list_and_values(t->l);
  }
  t->l=li;
  return l;
}

struct listAssoc* removeThatKey_listAssoc(struct listAssoc* l,char *key){
  if(l==NULL){
    return NULL;
  }
  
  struct listAssoc* res=l;
  if(!strcmp(key,l->k)){
    res=l->next;
    delNode_listAssoc(l);
    return res;
  }
  else{
    
    while(l->next){
      if(!strcmp(key,l->next->k)){
	break;
      }
      else{
	l=l->next;
      }
    }
    
    if(l==NULL){
      fprintf(stderr,"La liste ne contiens pas la clée %s\n",key);
    }
    else{
      struct listAssoc* t=l->next;
      if(t){
	l->next=t->next;
      }
      delNode_listAssoc(t);
    }
    return res;
  }
}

void delNode_listAssoc(struct listAssoc *l){
  if(l){
    free(l->k);
    delete_list_and_values(l->l);
    free(l);
  }
}
