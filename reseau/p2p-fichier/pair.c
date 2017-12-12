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

/*IPC*/
#include <sys/ipc.h>
#include <sys/msg.h>

/*erreur*/
#include <string.h>
#include <errno.h>

/*Manipulation dossier/fichiers*/
#include <dirent.h>
#include <sys/stat.h>

/*Threading*/
#include <pthread.h>

/*API personnelles*/
#include "listAssoc.h"
#include "shared_define.h"

struct servParam{
  int sock;
  pthread_t id_serv;
  int* compteur;
  int msgid;
  pthread_mutex_t m;
};

struct dllFile{
  char* adresse;
  uint16_t port;
  char *file;
};

struct sendpart{
  int sock;
  int msgid;
};

struct msgend{
  long label;
  pthread_t pid;
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
void IAMNOLONGERSERVER(struct servParam* s);
void ConnectToThatPeer(struct listAssoc* peer,uint16_t port);
void* getFileFromThatPeer(void* param);
void* sendfile(void* param);

/*Manipulation*/
int LookForEnd(char s[],int size);
char* ConvertBracketToStar(char s[],int fin,int size);
int CharPToInt(char* c,int size);
int getIntFromServ(int serv);


int main(int argc,char** argv){
  if(argc!=3){/*Check arguments*/
    fprintf(stderr,"\nUsage : ./p [adresse_annuaire] [Port_Annuaire]\n\n");
    exit(EXIT_FAILURE);
  }

  char* servAddress=argv[1];
  uint16_t port=htons(atoi(argv[2]));

  DisAuServeurQueJeSuisPresent(servAddress,port);
  
  struct servParam *sp=malloc(sizeof(struct servParam));
 
  pthread_mutex_init(&(sp->m),NULL);
  
  int k=0;
  if((k=ftok("./Seed",12))==-1){fprintf(stderr,"ftok failed : %s.\n",strerror(errno));};

  /*reset de la file de message*/
  if((sp->msgid=msgget(k,IPC_CREAT | 0666))==-1){fprintf(stderr,"creation fille de message failed : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  if(-1==msgctl(sp->msgid,IPC_RMID,NULL)){fprintf(stderr,"Probleme en essayant de détruire la file de message : %s",strerror(errno));}

  if((sp->msgid=msgget(k,IPC_CREAT | IPC_EXCL | 0666))==-1){fprintf(stderr,"creation fille de message failed : %s.\n",strerror(errno));exit(EXIT_FAILURE);}
  
  sp->sock=createServerSocket(port+1);/*Pour ne pas etre sur le meme port que le serveur annuaire*/
  sp->compteur=malloc(sizeof(int));
  *(sp->compteur)=0;
  sp->id_serv=0;/*Not usefull inside IAMSERVEURNOW, but avoid undefined behavior*/
  if(-1==pthread_create(&(sp->id_serv),NULL,IAMSERVEURNOW,(void*)sp)){
    fprintf(stderr,"problème creation thread serveur: %s.\n",strerror(errno));
  }
  

  
  struct listAssoc* list=NULL;
  list=RefreshThatList(servAddress,port);
  DisplayListAssoc(list);
  int done=0;
  int numPeer;
  struct listAssoc* peer=NULL;
  //struct list* file;
  char* query=malloc(sizeof(char)*10);/*Just in case, we'll only consider the first char anyway*/
  do{
    printf("\nC -> Connection à un pair\nR -> Refresh la liste de l'annuaire\nQ-> Quitte le reseau\n");

    if(NULL==fgets(query,10,stdin)){fprintf(stderr,"Problème lecture de la query : %s\n",strerror(errno));}
    switch(query[0]){
    case 'r':
    case 'R':
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
      if(!list){
	printf("Il n'y a aucun pairs connus pour l'instant, Je vous refresh la liste!\n");
	list=RefreshThatList(servAddress,port);
	DisplayListAssoc(list);
      }
      else{
	printf("Entrez le numéro du pair correspondant à la liste.\n");
	DisplayListAssoc(list);
	scanf("%d",&numPeer);
	fgetc(stdin);
	if(NULL!=(peer=getIndex_listAssoc(list,numPeer))){
	  printf("Connection au pair %d = %s.\nQuel est le numero du fichier que vous souhaitez télécharger?\n",numPeer,peer->k);
	  DisplayList(peer->l);
	  int numfile;
	  scanf("%d",&numfile);
	  fgetc(stdin);
	  struct list* filename;
	  if((filename=getIndex_list(peer->l,numfile))!=NULL){
	  
	    struct dllFile* p=malloc(sizeof(struct dllFile));
	    p->adresse=peer->k;
	    p->port=port+1;
	    p->file=filename->v;
	    getFileFromThatPeer((void*)p);/*pour ne pas etre au meme niveau que l'accés à l'annuaire*/
	  }
	  else{
	    printf("\n Numéro de fichier invalide.\n");
	  }
	}
	else{
	  printf("Ce chiffre ne correspond pas à un pair.\n");
	}
      }
      break;
    default:
      printf("Commande non reconnue.\n");
      break;
    }
      
  }while(!done);

  delete_listAssoc_and_key_and_values(list);


  DisAuServeurQueJeQuitte(servAddress,port);
  printf("J'ai dis au serveur annuaire que je partais\n");
  printf("J'envois le signal d'arret au serveur\n");
  
  pthread_mutex_lock(&(sp->m));
  IAMNOLONGERSERVER(sp);
  pthread_mutex_destroy(&(sp->m));
  free(sp);
  free(query);
  
  printf("J'ai tout finis, bonne journée:)\n");

  return 0;
}

int ConnectToServ(char* servAddress,uint16_t port){
  int sock=socket(PF_INET,SOCK_STREAM,0);//création de la socket
  if(sock==-1){fprintf(stderr,"Probleme ConnectToserv socket() : %d %s.\n",errno,strerror(errno));exit(EXIT_FAILURE);}
  
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
    if(errno==ECONNREFUSED){
      return -1;
    }
    else{
      fprintf(stderr,"problème ConnectToServ Connect() : %s.\n",strerror(errno));
      DisconnectFromServ(sock);
      exit(EXIT_FAILURE);
    }
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
  if(serv==-1){
    fprintf(stderr,"Serveur non accessible. Verifier qu'il soit allumé, et que sont adresse/port soient corrects\n");
    exit(EXIT_FAILURE);
  }
	       
  
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
      //printf("sending %s\n",buffer);
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
  if(serv==-1){
    fprintf(stderr,"Serveur non accessible. Verifier qu'il soit allumé, et que sont adresse/port soient corrects");
    exit(EXIT_FAILURE);
  }
  
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
  if(serv==-1){
    fprintf(stderr,"Serveur non accessible. Verifier qu'il soit allumé, et que sont adresse/port soient corrects");
    exit(EXIT_FAILURE);
  }

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
    
    if(buffer[0]=='\0'){
      break;
    }

    char* ip=resize(buffer,SIZE_BUFF);

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

      if(buffer[0]=='\0'){
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

void* getFileFromThatPeer(void* param){
  struct dllFile* p=(struct dllFile*)param;
  int serv=ConnectToServ(p->adresse,p->port);
  if(serv==-1){
    fprintf(stderr,"pair non accessible, a-t-il crash?\n");
    return NULL;
  }


  printf("Je demande le fichier %s\n",p->file);
  int len=0;
  while(p->file[len]!='\0'){len++;}//recherche taille du nom du fichier.

  char* buffer=malloc(SIZE_BUFF*sizeof(char));
  
  ssize_t res=send(serv,p->file,len+1,0);//Envois du nom du fichier au serveur.
  if(-1==res){
    fprintf(stderr,"problème sendName : %s.\n",strerror(errno));
    DisconnectFromServ(serv);
    free(buffer);
    free(p);
    exit(EXIT_FAILURE);
  }
 
  int* s=malloc(sizeof(int));
  res=recv(serv,s,sizeof(int),0);//Reception d'un certain nombre d'octet.
  if(-1==res){
    fprintf(stderr,"problème recv size file : %s.\n",strerror(errno));
    free(buffer);
    
    free(s);
    free(p);
    DisconnectFromServ(serv);
    exit(EXIT_FAILURE);
  }
  int size=*s;
  free(s);
  
  if(size!=0){//Overwise, it's done

    int j=0;
    while(buffer[j]!='\0'){//calcul de la longueur du nom
      j++;
    }

    char* filename=malloc(sizeof(char)*j+1+10);
    filename[0]='r';
    filename[1]='e';
    filename[2]='c';
    filename[3]='e';
    filename[4]='p';
    filename[5]='t';
    filename[6]='i';
    filename[7]='o';
    filename[8]='n';
    filename[9]='/';
    int i=10;
    j=0;
    while(p->file[j]!='\0'){
      filename[i]=p->file[j];
      ++i;++j;
    }
    filename[i]='\0';
    
    
    FILE* fileToReceive=fopen(filename,"w");//on ouvre en écriture le fichier que l'on va télécharger de ce serveur.
    if(fileToReceive==NULL){
      fprintf(stderr,"N'as pas pu ouvrir le fichier %s : %s.\n",p->file,strerror(errno));
      DisconnectFromServ(serv);
      free(buffer);
      free(filename);
      free(p);
      exit(EXIT_FAILURE);
    }



    int som=0;//Nombre d'octet reçu.
    while(1){//Boucle infinie, la condition de fin est géré dedans.
    
      ssize_t res=recv(serv,buffer,SIZE_BUFF,0);//Reception d'un certain nombre d'octet.
      if(-1==res){
	fprintf(stderr,"problème recv : %s.\n",strerror(errno));
	free(buffer);
	free(p);
	free(filename);
	DisconnectFromServ(serv);
	exit(EXIT_FAILURE);
      }

      if(res!=0){//Et on l'écrit dans le fichier.
	som+=res;//On ajoute le nombre d'octet reçu par le dernier envois du serveur.
	ssize_t r=fwrite(buffer,1,res,fileToReceive);
	if(res!=r){//en verifiant qu'il n'y ai pas de problème du write
	  printf("weird write\n");
	}
      }
      
      if(som==size){//Output du nombre d'octect reçu
	printf("%d octets received, fichier correctement reçu.\n",som);
	break;
      }
      
    }
    
  }
  
  
  free(buffer);
  free(param);
  
  DisconnectFromServ(serv);  
  return NULL;
}


void getThreads(struct servParam* s,int waitflag){
  struct msgend buff;
  
  while(*(s->compteur)){
    if(msgrcv(s->msgid,&buff,sizeof(struct msgend),0,waitflag)==-1){
      if(errno==ENOMSG){
	break;/*No other threads are done*/
      }
      else{
	fprintf(stderr,"probleme récupération des threads finis : %s.\n",strerror(errno));
	exit(EXIT_FAILURE);
      }
    }
    
    pthread_mutex_lock(&(s->m));
    pthread_join(buff.pid,NULL);
    printf("J'ai attrapé un thread qui a finis ! état compteur = %d\n",*(s->compteur));
    *(s->compteur)-=1;
    pthread_mutex_unlock(&(s->m));
  }
}

void getOneThread(struct servParam* s){
  struct msgend buff;
  if(msgrcv(s->msgid,&buff,sizeof(struct msgend),0,0)==-1){fprintf(stderr,"probleme récupération des threads finis : %s.\n",strerror(errno));}
  pthread_mutex_lock(&(s->m));
  pthread_join(buff.pid,NULL);
  *(s->compteur)-=1;
  pthread_mutex_unlock(&(s->m));
}

void* IAMSERVEURNOW(void* param){
  struct servParam* p=(struct servParam*)param;
  pthread_t t;/*don't need it with that algorithm*/
  int x;
  while(1){
    if(-1==(x=accept(p->sock,NULL,NULL)))
      fprintf(stderr,"Accept failed : %d - %s.\n",errno,strerror(errno));
    
    getThreads(p,IPC_NOWAIT);//On verifie si certains thread on finis de bosser, pour éviter de trop flood
    
    printf("accept %d \n",x);
    struct sendpart *s=malloc(sizeof(struct sendpart));
    s->sock=x;
    s->msgid=p->msgid;

    pthread_mutex_lock(&(p->m));
    pthread_create(&t,NULL,sendfile,(void*)s);
    *(p->compteur)+=1;
    pthread_mutex_unlock(&(p->m));
  }
  
  pthread_exit(NULL);//cancel dans le main thread, n'arriveras jamais ici.
}


void IAMNOLONGERSERVER(struct servParam* s){
  if(pthread_cancel(s->id_serv)==-1)fprintf(stderr,"problème close sockListen : %s.\n",strerror(errno));
  if(close(s->sock)==-1)fprintf(stderr,"problème close sockListen : %s.\n",strerror(errno));

  printf("J'attends que toutes les taches du serveurs soient finies.\n");
  pthread_mutex_unlock(&(s->m));
  getThreads(s,0);
  printf("Le serveur à finis toutes ses taches\n");
  free(s->compteur);

  if(-1==msgctl(s->msgid,IPC_RMID,NULL)){fprintf(stderr,"Probleme en essayant de détruire la file de message : %s",strerror(errno));}
}


void* sendfile(void* param){
  struct sendpart* p=(struct sendpart*)param;
  int descClient=p->sock;

  char *buffer=malloc(sizeof(char)*SIZE_BUFF);//Préparation du buffer d'envois

  ssize_t res=recv(descClient,buffer,SIZE_BUFF,0);//Reception du nom
  if(-1==res){
    fprintf(stderr,"problème recv filename : %s.\n",strerror(errno));
    close(descClient);
    exit(EXIT_FAILURE);
  }

  int i=0;
  while(buffer[i]!='\0'){//calcul de la longueur du nom
    i++;
  }

  printf("Filename demandé = %s size = %d\n",buffer,i);
  
  char* filename=malloc(sizeof(char)*i+1+5);
  filename[0]='S';
  filename[1]='e';
  filename[2]='e';
  filename[3]='d';
  filename[4]='/';
  int j=5;
  i=0;
  while(buffer[i]!='\0'){
    filename[j]=buffer[i];
    ++i;++j;
  }
  filename[j]='\0';

  int* s=malloc(sizeof(int));
  struct stat st;
  if(-1==stat(filename, &st)){//File doesn't exist
    printf("error stat %d error : %s\n",errno,strerror(errno));
    printf("Fichier demandé : %s n'existe pas\n",filename);

    *s=0;
    res=send(descClient,s,sizeof(int),0);//signal de fichier_doesn't exist
    if(-1==res){
      fprintf(stderr,"problème send fichier %s n'existe pas : %s.\n",filename,strerror(errno));
      close(descClient);
      free(buffer);
      free(filename);
      free(param);
      free(s);
      pthread_exit(NULL);
    }
    else{
      free(s);
    }
  }
  else{//File exist, on l'envoie
    printf("Envois du fichier %s\n",filename);
    *s=(int)st.st_size;
    res=send(descClient,s,sizeof(int),0);//Envois taille du fichier
    if(-1==res){
      fprintf(stderr,"problème send fichier %s pas : %s.\n",filename,strerror(errno));
      close(descClient);
      free(buffer);
      free(s);
      exit(EXIT_FAILURE);
    }
    
    FILE* fileToSend=fopen(filename,"r");//Ouverture du fichier.
    if(fileToSend==NULL){//NOT SUPPOSED TO HAPPEND, since we checked with stat
      fprintf(stderr,"file failed to open even though it was supposed to exist : %s.\n",strerror(errno));
      close(descClient);
      free(buffer);
      remove(filename);//On retire le fichier vide nouvellement créé.
    }

    printf("Je commence à envoyer!\n");
    
    int c;
    ssize_t som=0;
    while(1){//sinon il envoit le fichier.
      i=0;
      while(i<SIZE_BUFF){//On remplis le buffer.
	c=fgetc(fileToSend);
	if(c==EOF){
	  break;
	}
	buffer[i]=c;
	i++;
      }
      
      res=send(descClient,buffer,i,0);//Et on l'envois, jusqu'à la taille maximale, ou la fin du fichier
      if(-1==res){
	fprintf(stderr,"problème send : %s.\n",strerror(errno));
	close(descClient);
	free(buffer);
	free(filename);
	free(param);
	fclose(fileToSend);
	exit(EXIT_FAILURE);
      }
      else{
	som+=res;
      }
      if(feof(fileToSend))//fin du fichier atteinte, on a finis.
	break;
    }

    fclose(fileToSend);
    printf("done sending a file.\n");
    close(descClient);
  }

  free(buffer);
  free(filename);

  struct msgend mess;
  mess.label=2;/*doesn't matter*/
  mess.pid=pthread_self();
  if(msgsnd(p->msgid,&mess,(size_t)sizeof(struct msgend),0)){
    fprintf(stderr,"Erreur en mettant le message de fin de thread : %s",strerror(errno));
  }
  
  free(param);
  pthread_exit(NULL);
}
 
