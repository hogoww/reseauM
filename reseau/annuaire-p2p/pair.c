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

/*Threading*/
#include <pthread.h>

/*API personnelles*/
#include "listAssoc.h"
#include "shared_define.h"

struct servParam{
  int sock;
  pthread_t id_serv;
  char* buffer;
};


/*Connection*/
int ConnectToServ(char* servAddress,uint16_t port);
void DisconnectFromServ(int sock);

/*interaction Annuaire - pair*/
void DisAuServeurQueJeSuisPresent(char* servAddress,uint16_t port);
void DisAuServeurQueJeQuitte(char* servAddress,uint16_t port);
void QueryTypeServ(int serv,char type);
struct listAssoc* RefreshThatList(char* servAddress,uint16_t port);

/*peer to peer side*/
int createServerSocket(uint16_t port);
void* IAMSERVEURNOW(void* param);
void IAMNOLONGERSERVER(struct servParam* p);
void ConnectToThatPeer(struct listAssoc* peer,uint16_t port);

/*Manipulation*/
int LookForEnd(char s[],int size);
char* ConvertBracketToStar(char s[],int fin,int size);
int CharPToInt(char* c,int size);
int getIntFromServ(int serv);


int main(int argc,char** argv){
  if(argc!=3){/*Check arguments*/
    fprintf(stderr,"\nUsage : ./p [adresse_annuaire] [Port_Annuaire] \n\n");
    exit(EXIT_FAILURE);
  }

  char* servAddress=argv[1];
  uint16_t port=htons(atoi(argv[2]));

  DisAuServeurQueJeSuisPresent(servAddress,port);
  
  pthread_t server;/*Lancement serveur*/
  struct servParam *sp=malloc(sizeof(struct servParam));
  sp->sock=createServerSocket(port+1);/*Pour ne pas etre sur le meme port que le serveur annuaire*/
  sp->buffer=malloc(sizeof(char)*SIZE_BUFF);
  if(-1==pthread_create(&server,NULL,IAMSERVEURNOW,(void*)sp)){
    fprintf(stderr,"problème creation thread serveur: %s.\n",strerror(errno));
  }
  sp->id_serv=server;
  
  struct listAssoc* list=NULL;
  list=RefreshThatList(servAddress,port);
  DisplayListAssoc(list);
  int done=0;
  int numPeer;
  struct listAssoc* peer=NULL;
  //struct list* file;
  do{
    printf("\nC -> Connecte à un pair\nR -> Refresh la liste de l'annuaire\nQ -> Quitte le reseau\n");
    
    switch(fgetc(stdin)){
      fgetc(stdin);/*Remove the newline char we don't need*/
    case 'r':
    case 'R':
      fgetc(stdin);
      printf("Refresh de la liste des pairs\n");
      delete_listAssoc_and_key_and_values(list);
      list=RefreshThatList(servAddress,port);
      DisplayListAssoc(list);
      break;
    case 'q':
    case 'Q':
      done=1;
      break;
    case 'c':
    case 'C':
      printf("Entrez le numéro du pair correspondant à la liste.\n");
      DisplayListAssoc(list);
      scanf("%d",&numPeer);
      fgetc(stdin);/*Remove the newline char we don't need*/
      if((peer=getIndex_listAssoc(list,numPeer))){
	printf("Connection au pair %d = %s.\n",numPeer,peer->k);
	ConnectToThatPeer(peer,port+1);/*pour ne pas etre au meme niveau que l'accés à l'annuaire*/
      }
      else{
	printf("Ce chiffre ne correspond pas à un pair.\n");
      }
      break;
      
    default:
      printf("Commande non reconnue.\n");
      break;
    }
      
  }while(!done);

  delete_listAssoc_and_key_and_values(list);
  IAMNOLONGERSERVER(sp);
  printf("serveur arrété.\n");
  DisAuServeurQueJeQuitte(servAddress,port);
  printf("J'ai dis au serveur annuaire que je partais.\n");

  printf("J'ai tout finis, je m'arrète, bonne journée ! \n");

  return 0;
}

int ConnectToServ(char* servAddress,uint16_t port){
  int sock=socket(PF_INET,SOCK_STREAM,0);//création de la socket
  if(sock==-1){fprintf(stderr,"Probleme ConnectToserv socket() : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  struct sockaddr_in addr;//et remplissage des infos passé en ligne de commande.
  memset(&addr,0,sizeof(struct sockaddr));
  addr.sin_family=AF_INET;

  int checkReturn=inet_pton(AF_INET,servAddress,&(addr.sin_addr));
  if (checkReturn==0 || checkReturn==-1){
    fprintf(stderr, "ConnectToServ : Adresse fournie format incorrect ; %s.\n",strerror(errno));
    DisconnectFromServ(sock);
    exit(EXIT_FAILURE);
  }
  addr.sin_port=port;

  if(-1==connect(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr))){//Connection au serveur
    fprintf(stderr,"problème ConnectToServ Connect() : %s.\n",strerror(errno));
    DisconnectFromServ(sock);
    exit(EXIT_FAILURE);
  }
  
  return sock;
}

void DisconnectFromServ(int sock){
  if(-1==close(sock)){fprintf(stderr,"Probleme DisconnectToServ socket() : %s.\n",strerror(errno));}
}

int LookForEnd(char s[],int size){
  int i=0;
  while(i<size && s[i]!='\0'){++i;}
  return i;
}

char* ConvertBracketToStar(char s[],int end,int size){
  if(end>size){
    fprintf(stderr,"Convert char[] to char* failed\n");
  }
  int i=0;
  char* res=malloc(sizeof(char)*end);
  if(res==NULL){fprintf(stderr,"Problème Malloc : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  while(i<end){
    res[i]=s[i];
    i++;
  }
  res[i]='\0';
  return res;
}

int getIntFromServ(int serv){

  char* e=malloc(sizeof(char)*32);/*max int = pow(2,32)*/
  ssize_t res=recv(serv,e,32,0);/*envoie le type de query sur le serv'*/
  if(-1==res){
    fprintf(stderr,"problème getIntFromServ : %s.\n",strerror(errno));
    DisconnectFromServ(serv);
    exit(EXIT_FAILURE);
  }
  int r=atoi(e);/*\0 doit avoit été mis correctement coté serv'.*/
  free(e);
  return r;
}

void DisAuServeurQueJeSuisPresent(char* servAddress,uint16_t port){
  int serv=ConnectToServ(servAddress,port);

  QueryTypeServ(serv,COMING);
  char *buffer=malloc(sizeof(char)*SIZE_BUFF);
  DIR* d;
  struct dirent* dir;
  d=opendir("./Seed");
  if(d){
    while ((dir = readdir(d)) != NULL){
      if((dir->d_name[0]=='.')){
	continue;/*Pour ne pas afficher les fichier cachés*/
      }

      int end=LookForEnd(dir->d_name,256);
      char* r=ConvertBracketToStar(dir->d_name,end,256);
      strcpy(buffer,r);
      printf("sending %s\n",buffer);
      ssize_t res=send(serv,buffer,SIZE_BUFF,0);//Envois du nom du fichier au serveur.
      
      if(-1==res){
	fprintf(stderr,"DisAuServeurQueJeSuisPresent() problème envoie d'un nom de fichier : %s.\n",strerror(errno));
	closedir(d);
	DisconnectFromServ(port);
	exit(EXIT_FAILURE);
      }
      free(r);
    }

    buffer[0]='\0';
    ssize_t res=send(serv,buffer,0,0);//fin des fichiers à réceptionner.
    if(-1==res){
      fprintf(stderr,"problème DisAuServeurQueJeSuisPresent() envoie signal fin de fichier : %s.\n",strerror(errno));
      closedir(d);
      DisconnectFromServ(serv);    
      exit(EXIT_FAILURE);
    }

    closedir(d);
  }
  else{
    fprintf(stderr,"DisAuServeurQueJeSuisPresent Le repertoire n'as pas été ouvert : %s.\n",strerror(errno));
    DisconnectFromServ(serv);
    exit(EXIT_FAILURE);
  }

  free(buffer);
  DisconnectFromServ(serv);
}
  

void QueryTypeServ(int serv,char type){
  char* e=malloc(sizeof(char));
  *e=(char)type;/*cast suffisant vu que le serveur le considèreras de la meme façon.*/
  ssize_t res=send(serv,e,1,0);/*envoie le type de query sur le serv'*/
  if(-1==res){
    fprintf(stderr,"problème DisAuServeurQueJeQuitte() queryTypeServ : %s.\n",strerror(errno));
    DisconnectFromServ(serv);    
    exit(EXIT_FAILURE);
  }
  free(e);
}


void DisAuServeurQueJeQuitte(char* servAddress,uint16_t port){
  int serv=ConnectToServ(servAddress,port);
  
  QueryTypeServ(serv,LEAVING);
  
  DisconnectFromServ(serv);
}

int CharPToInt(char* c,int size){
  int end=LookForEnd(c,size);
  char* res=malloc(sizeof(char)*end+1);
  int i=0;
  while(i<end){
    res[i]=c[i];
    ++i;
  }
  res[i]='\0';
  return atoi(res);
}

char* resize(char* s,int size){
  int end=LookForEnd(s,size);
  if(end>size){
    fprintf(stderr,"Didn't find a \\0 before i was out of char to check");
    exit(EXIT_FAILURE);
  }
  char* res=malloc(sizeof(char)*end+1);
  int i=0;
  while(i<end){
    res[i]=s[i];
    ++i;
  }
  res[i]='\0';
  return res;
}


struct listAssoc* RefreshThatList(char* servAddress,uint16_t port){/*Not the most optimize, but that'll be enough*/
  int serv=ConnectToServ(servAddress,port);
  QueryTypeServ(serv,REFRESH);
  
  
  char* buffer=malloc(sizeof(char)*SIZE_BUFF);

  struct listAssoc* list=NULL;
  ssize_t res1,res2;
  
  do{
    res1=recv(serv,buffer,SIZE_BUFF,0);//Reception du nom du ième pair
    if(-1==res1){
      fprintf(stderr,"problème recv pair name, RefreshThatList : %s.\n",strerror(errno));
      free(buffer);
      DisconnectFromServ(serv);
      exit(EXIT_FAILURE);
    }
    
    if(res1==0 || buffer[0]=='\0'){
      break;
    }

    char* ip=resize(buffer,SIZE_BUFF);
    printf("ip=%s\n",ip);

    if(!list){
      list=make_ListAssoc(ip);
    }
    
    struct list* lt=NULL;
   
    do{
      res2=recv(serv,buffer,SIZE_BUFF,0);//Reception du jeme filename du ieme pair
      if(-1==res2){
	fprintf(stderr,"problème recv fileName RefreshThatList : %s.\n",strerror(errno));
	free(buffer);
	DisconnectFromServ(serv);
	exit(EXIT_FAILURE);
      }
      
      if(res2==0 || buffer[0]=='\0'){
	break;
      }
      
      printf("recv filename %s\n",buffer);
      char* fileName=resize(buffer,SIZE_BUFF);
      lt=add_value_list(lt,fileName);
    }while(1);
    
    destroyAndChangeList_listAssoc(list,ip,lt);

  }while(1);

  free(buffer);
  
  DisconnectFromServ(serv);
  return list;
}



int createServerSocket(uint16_t port){
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
  
  return sockListen;
}

void* IAMSERVEURNOW(void* param){
  struct servParam* p=(struct servParam*)param;
  
  printf("Server thread standing by !\n");
  while(1){
    printf("waiting for new client\n");
    int x=accept(p->sock,NULL,NULL);
    printf("accepté %d \n",x);
    
    printf("J'attends qu'il se déconnecte quelque chose avant de m'occuper du suivant.\n(Il n'est pas précisé dans les specs qu'on doit pouvoir en traiter plusieurs à la fois)\n");
    printf("Ne quittez pas svp\n");
    if(-1==recv(x,p->buffer,SIZE_BUFF,0))//Attente passive, recv renveras 0 lorsque le client se seras déconnecté. 
       fprintf(stderr,"problème receive serv : %s.\n",strerror(errno));
    printf("finis avec client %d\n",x);
  }
}


void IAMNOLONGERSERVER(struct servParam* s){
  if(!pthread_cancel(s->id_serv))fprintf(stderr,"problème cancel thread serveur : %s.\n",strerror(errno));
  if(close(s->sock)==-1)fprintf(stderr,"problème close sockListen : %s.\n",strerror(errno));
  free(s->buffer);
}


void ConnectToThatPeer(struct listAssoc* peer,uint16_t port){
  int serv=ConnectToServ(peer->k,port);

  
  printf("appuyez sur la touche entrée pour vous déconnecter.\n");
  char* b=malloc(1024);
  fgets(b,1024,stdin);
  free(b);
  printf("Vous allez manquer à %s :(",peer->k);
  
  DisconnectFromServ(serv);
}
