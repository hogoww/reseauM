/*classique*/
#include <unistd.h>/*chdir*/
#include <stdio.h>
#include <stdlib.h>

/*reseau*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*erreur*/
#include <string.h>
#include <errno.h>

/*Manipulation dossier*/
#include <dirent.h>

/*fin du serveur*/
#include <signal.h>

/*API personnelles*/
#include "listAssoc.h"
#include "shared_define.h"

int getQueryFromPeer(int descClient);
void sigINT_handler(int signo);
struct listAssoc* treatQuery(int descClient,int q,struct listAssoc* list);
struct listAssoc* AddThatClient(int descClient,struct listAssoc* list);
void SendListToThatClient(int descClient,struct listAssoc* list);
struct listAssoc* RemoveThatClient(int descClient,struct listAssoc* list);
char* getClientIPString(int descClient);


char* buffer=NULL;
int sockListen;

int main(int argc,char** argv){
  if(argc!=2){/*Check arguments*/
    fprintf(stderr,"\nusage : ./p [port] \n\n");
    exit(EXIT_FAILURE);
  }

  if(signal(SIGINT, sigINT_handler) == SIG_ERR){//Pour une fermeture propre du serveur.
    fprintf(stderr,"Pas réussi à attribuer au signal INT sa trap\n");
    exit(EXIT_FAILURE);
  }

  uint16_t port=htons(atoi(argv[1]));

  int sockListen=socket(PF_INET,SOCK_STREAM,0);//création de la socket

  struct sockaddr_in addr;//et remplissage
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=INADDR_ANY;
  addr.sin_port=port;
  
  int checkBind=bind(sockListen,(struct sockaddr*)&addr,sizeof(struct sockaddr));//binding de la socket.
  if(-1==checkBind){
    if(errno==EADDRINUSE){
      fprintf(stderr,"L'OS ne tolère pas que le port du socket soit identique entre deux exécutions proches.\nPour plus d'informations sur pourquoi cette attente : \n\nhttps://stackoverflow.com/questions/775638/using-so-reuseaddr-what-happens-to-previously-open-socket\n\n");
    }
    else{
      fprintf(stderr,"problème bind : %d %s.\n",errno,strerror(errno));
    }
    exit(EXIT_FAILURE);
  }
  
  if(-1==listen(sockListen,1)){//pour mettre la socket en passif.
    fprintf(stderr,"problème listen : %s.\n",strerror(errno));
    exit(EXIT_FAILURE);
  }
  
  struct listAssoc* list=NULL;
 
  printf("Serveur dispo. Utilisez le signal ctrl+c pour l'arreter proprement\n");

  while(1){//On commence la boucle infinie de reception, qui ne se termineras Proprement que sur un sigINT (ctrl + c)
    printf("En attente d'un nouveau client\n");
    
    struct sockaddr_in client;
    socklen_t s=sizeof(struct sockaddr_in);
    int descClient=accept(sockListen,(struct sockaddr*)&client,&s);//acceptation du nouveau client (ou attente de celui ci, vu que accept est bloquant).
    if(-1==descClient){
      fprintf(stderr,"problème accept : %s.\n",strerror(errno));
      exit(EXIT_FAILURE);
    }
    /* getIP(client,s); */
    int q=getQueryFromPeer(descClient);
    list=treatQuery(descClient,q,list);
    
    close(descClient);
  }
 
  exit(EXIT_SUCCESS);
}


void sigINT_handler(int signo){
  if(signo == SIGINT){
    printf("Arret propre du serveur, bonne journée :)\n");
    close(sockListen);
    exit(EXIT_SUCCESS);
  }
}

int getQueryFromPeer(int descClient){
  char* buffer=malloc(sizeof(char));
  
  ssize_t res=recv(descClient,buffer,1,0);//Reception du nom
  if(-1==res){
    fprintf(stderr,"problème get query : %s.\n",strerror(errno));
    free(buffer);
    close(descClient);
    exit(EXIT_FAILURE);
  }
  
  int a=(int)*buffer;
  free(buffer);
  return a;
}

struct listAssoc* treatQuery(int descClient,int q,struct listAssoc* list){
  char* ip;
  switch(q){
  case LEAVING:
    ip=getClientIPString(descClient);
    printf("removing %s\n",ip);
    list=RemoveThatClient(descClient,list);
    free(ip);
    DisplayListAssoc(list);
    break;
  case COMING:
    list=AddThatClient(descClient,list);
    DisplayListAssoc(list);
    SendListToThatClient(descClient,list);
    break;
  case REFRESH:
    ip=getClientIPString(descClient);
    printf("refreshing for %s\n",ip);
    SendListToThatClient(descClient,list);
    break;
  default:
    fprintf(stderr,"Requete client n'as pas été reconnue, je ferme sa connection\n");
  }
  return list;
}

struct listAssoc* AddThatClient(int descClient,struct listAssoc* list){
  char* ip=getClientIPString(descClient);
  printf("J'ajoute le client d'ip %s \n",ip);
  
  buffer=malloc(sizeof(char)*SIZE_BUFF);
  struct list* l=NULL;
  ssize_t res=1;
  
  do{
    res=recv(descClient,buffer,SIZE_BUFF,0);
    if(-1==res){
      fprintf(stderr,"problème reception nom fichier d'un nouveau client : %s.\n",strerror(errno));
      free(buffer);
      close(descClient);
      exit(EXIT_FAILURE);
    }
    
    if(res==0 || buffer[0]=='\0'){
      break; 
    }
    
    char* t=strcpy(malloc(sizeof(char)*res),buffer);/*libéré en meme temps que la liste*/
    printf("%s\n",buffer);
    l=add_value_list(l,t);
  }
  while(1);

  
  list=destroyAndChangeList_listAssoc(list,ip,l);
  //DisplayListAssoc(list);

  free(buffer);
  return list;
}

struct listAssoc* RemoveThatClient(int descClient,struct listAssoc* list){
  return removeThatKey_listAssoc(list,getClientIPString(descClient));
}

char* getClientIPString(int descClient){
  struct sockaddr_in client;
  socklen_t s=sizeof(client);
  if(-1==getpeername(descClient,(struct sockaddr*)&client,&s)){
    fprintf(stderr,"getpeername failed : %s\n",strerror(errno));
  }
  char* ip=malloc(INET_ADDRSTRLEN*sizeof(char));/*seras free en meme temps que la liste*/
  if(NULL==inet_ntop(AF_INET, &(client.sin_addr), ip, sizeof(char)*INET_ADDRSTRLEN)){fprintf(stderr,"inet_ntop failed : %s \n",strerror(errno));}
  
  return ip;
}

void SendListToThatClient(int descClient,struct listAssoc* list){
  char* ip=getClientIPString(descClient);
  char* buffer=malloc(sizeof(char)*SIZE_BUFF);

  ssize_t res1,res2;
  
  while(list!=NULL){
    if(list==NULL || !strcmp(list->k,ip)){
      /* printf("skipping client %s\n",ip); */
      list=list->next;
      continue;/*On n'envois pas la listes des fichiers de celui qui la demande*/
      /*Ni les pairs ne partageant pas de fichiers*/
    }
    

    strcpy(buffer,list->k);
    printf("sending %s\n",buffer);
    res1=send(descClient,buffer,SIZE_BUFF,0);//envoie du nom du ième pair
    if(-1==res1){
      fprintf(stderr,"problème send pair name, RefreshThatList : %s.\n",strerror(errno));
      free(buffer);
      free(ip);
      close(descClient);
      exit(EXIT_FAILURE);
    }
    
    struct list* t=list->l;
    while(t!=NULL){
      strcpy(buffer,t->v);
      printf("sending %s\n",buffer);
      res2=send(descClient,buffer,SIZE_BUFF,0);//envoi du jeme fichier ieme pair
      if(-1==res2){
	fprintf(stderr,"problème send fileName RefreshThatList : %s.\n",strerror(errno));
	free(buffer);
	free(ip);
	close(descClient);
	exit(EXIT_FAILURE);
      }
      t=t->next;
    }
    
    buffer[0]='\0';
    res1=send(descClient,buffer,SIZE_BUFF,0);//fin fichier du ieme pair
    if(-1==res1){
      fprintf(stderr,"problème send fileName RefreshThatList : %s.\n",strerror(errno));
      free(buffer);
      free(ip);
      close(descClient);
      exit(EXIT_FAILURE);
    }      
    buffer[0]='1';
    
    list=list->next;
  }
  
  buffer[0]='\0';
  res1=send(descClient,buffer,SIZE_BUFF,0);//envois du signal de fin du ieme pair
  if(-1==res1){
    fprintf(stderr,"problème send fileName RefreshThatList : %s.\n",strerror(errno));
    free(buffer);
    free(ip);
    close(descClient);
    exit(EXIT_FAILURE);
  }
  

  free(ip);
  free(buffer);
}
