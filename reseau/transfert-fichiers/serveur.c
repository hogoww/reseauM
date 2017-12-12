#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define SIZE_BUFF 4096

static int receiving=0;
int sockListen;
int descClient;
FILE* fileToSend;  
char* buffer;


void sigINT_handler(int signo){

  if(signo == SIGINT){
    if(receiving){//fermeture dans le fils si il est en train de travailler
      printf("\nkill me when i'm working ain't nice!\n");
      printf("closing though\n");
      close(descClient);
      fclose(fileToSend);
    }//Et fermeture du reste sinon (père + fils)
    else{
      printf("\nClosing, have a nice day :)\n");
    }
    if(close(sockListen)==-1)fprintf(stderr,"problème close sockListen : %s.\n",strerror(errno));
    if(buffer!=NULL)
      free(buffer);
    exit(EXIT_SUCCESS);
  }
}

int main(int argc,char** argv){
  if(argc!=2){//Verification du nombre d'arguments
    fprintf(stderr,"usage : ./serveur port\n");
    exit(EXIT_FAILURE);
  }

 
  if(signal(SIGINT, sigINT_handler) == SIG_ERR){//Pour une fermeture propre du serveur.
    fprintf(stderr,"Pas réussi à attribuer au signal INT sa trap\n");
    exit(EXIT_FAILURE);
  }

  int check=chdir("emission");//Pour se placer directement dans le repertoire où les fichiers sont stockés.
  if(check==-1){
    fprintf(stderr,"problème chdir : %s.\n",strerror(errno));
    exit(EXIT_FAILURE);
  }

  sockListen=socket(PF_INET,SOCK_STREAM,0);//création de la socket

  struct sockaddr_in addr;//et remplissage
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=INADDR_ANY;
  addr.sin_port=htons(atoi(argv[1]));
  
  
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
  
  char* fileName;
  while(1){//On commence la boucle infinie de reception, qui ne se termineras Proprement que sur un sigINT (ctrl + c)
    
    printf("waiting for a new client\n");
    descClient=accept(sockListen,NULL,NULL);//acceptation du nouveau client (ou attente de celui ci, vu que accept est bloquant).
    if(-1==descClient){
      fprintf(stderr,"problème accept : %s.\n",strerror(errno));
      exit(EXIT_FAILURE);
    }
    
    /* struct sockaddr_in client; */
    /* socklen_t s=sizeof(struct sockaddr_in); */
    /* getpeername(descClient,(struct sockaddr*)&client,&s); */
    /* char* cl=malloc(100*sizeof(char)); */
    /* inet_ntop(AF_INET, &client, cl, sizeof(char)*100); */
  
    /* printf("sending a file to %s \n",cl); */
    printf("sending a file \n");

    buffer=malloc(sizeof(char)*SIZE_BUFF);//Préparation du buffer d'envois
    memset(buffer,0,SIZE_BUFF);
    
    ssize_t res=recv(descClient,buffer,SIZE_BUFF,0);//Reception du nom
    if(-1==res){
      fprintf(stderr,"problème recv fileName : %s.\n",strerror(errno));
      free(buffer);
      close(descClient);
      exit(EXIT_FAILURE);
    }

    int j=0;
    while(buffer[j]!='\0'){//calcul de la longueur du nom
      j++;
    }
    
    fileName=malloc(sizeof(char)*j+1);
    
    ssize_t i=0;
    while(buffer[i]!='\0'){
      fileName[i]=buffer[i];
      i++;
    }
    fileName[j]='\0';
    
    
    printf("\n%s\n",fileName);//Notification sur la sortie standards du nom du fichier à envoyer
      
    fileToSend=fopen(fileName,"r");//Ouverture du fichier.
    if(fileToSend==NULL){//Si le fichier n'existe pas, le fils s'arrète.
      fprintf(stderr,"file failed to open : %s.\n",strerror(errno));
      close(descClient);
      free(buffer);
      buffer=NULL;
      remove(fileName);//On retire le fichier vide nouvellement créé.
      continue;
    }
	
    int c;
    while(1){//sinon il envoit le fichier.
      i=0;
      while(i<SIZE_BUFF){//On remplis le buffer.
	c=fgetc(fileToSend);
	if(c==EOF)
	  break;
	buffer[i]=c;
	i++;
      }


      ssize_t res=send(descClient,buffer,i,0);//Et on l'envois, jusqu'à la taille maximale, ou la fin du fichier
      if(-1==res){
	fprintf(stderr,"problème send : %s.\n",strerror(errno));
	close(descClient);
	free(buffer);
	fclose(fileToSend);
	exit(EXIT_FAILURE);
      }
      
      
      if(feof(fileToSend)){//fin du fichier atteinte, on a finis.
	if(i!=0){

	  ssize_t res=send(descClient,buffer,0,0);//Et on l'envois, jusqu'à la taille maximale, ou la fin du fichier
	  if(-1==res){
	    fprintf(stderr,"problème send : %s.\n",strerror(errno));
	    close(descClient);
	    free(buffer);
	    fclose(fileToSend);
	    exit(EXIT_FAILURE);
	  }
	}
	break;
      }
    }
    free(buffer);
    buffer=NULL;
    printf("done sending a file\n");
    close(descClient);
  }

  //N'est pas supposé arrivé
  exit(EXIT_FAILURE);
}
