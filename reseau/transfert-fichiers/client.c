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
#include <unistd.h>


#define SIZE_BUFF 4096




int main(int argc,char** argv){
  if(argc!=4){//Vérification nombre d'arguments
    fprintf(stderr,"usage : ./client adresse port fileName\n");
    exit(EXIT_FAILURE);
  }
  

  int sock=socket(PF_INET,SOCK_STREAM,0);//création de la socket


  struct sockaddr_in addr;//et remplissage des infos passé en ligne de commande.
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  int checkReturn=inet_pton(AF_INET,argv[1],&(addr.sin_addr));
  if (checkReturn == 0){
    fprintf(stderr, "Adresse fournie format incorrect");
    close(sock);
    exit(EXIT_FAILURE);
  }
  addr.sin_port=htons(atoi(argv[2]));
  
  
  int checkConnect=connect(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr));//Connection au serveur
  if(-1==checkConnect){
    fprintf(stderr,"problème connect : %s.\n",strerror(errno));
    close(sock);
    exit(EXIT_FAILURE);
  }


  
  char* buffer=NULL;//préparation buffer
  buffer=malloc(sizeof(char)*SIZE_BUFF); 
  memset(buffer,0,SIZE_BUFF);


  int len=0;
  while(argv[3][len]!='\0'){len++;}//recherche taille du nom du fichier.

  ssize_t res=send(sock,argv[3],len+1,0);//Envois du nom du fichier au serveur.
  if(-1==res){
    fprintf(stderr,"problème sendName : %s.\n",strerror(errno));
    close(sock);
    free(buffer);
    exit(EXIT_FAILURE);
  }

  int check=chdir("reception");//On se met dans le bon dossier de reception
  if(check==-1){
    fprintf(stderr,"problème chdir : %s.\n",strerror(errno));
    free(buffer);
    close(sock);
    exit(EXIT_FAILURE);
  }
 
  
  FILE* fileToReceive=fopen(argv[3],"w");//on ouvre en écriture le fichier que l'on va télécharger de ce serveur.
  if(fileToReceive==NULL){
    fprintf(stderr,"file failed to open : %s.\n",strerror(errno));
    close(sock);
    free(buffer);
    exit(EXIT_FAILURE);
  }

  
  int som=0;//Nombre d'octet reçu.
  while(1){//Boucle infinie, la condition de fin est géré dedans.

    ssize_t res=recv(sock,buffer,SIZE_BUFF,0);//Reception d'un certain nombre d'octet.
    if(-1==res){
      fprintf(stderr,"problème recv : %s.\n",strerror(errno));
      //add failure
      
      exit(EXIT_FAILURE);
    }

    som+=res;//On ajoute le nombre d'octet reçu par le dernier envois du serveur.
    if(res!=0){//Et on l'écrit dans le fichier.
     
      ssize_t r=fwrite(buffer,1,res,fileToReceive);
      if(res!=r){//en verifiant qu'il n'y ai pas de problème du write
	printf("weird write\n");
      }
    }

    if(res==0){//On verifie si on est en fin de fichier
      break;
    }

  }

  if(som){//Output du nombre d'octect reçu
    printf("%d octets received\n",som);
  }
  else{//N'as rien reçu, et efface donc le fichier créé dans le dossier de reception, étant donné qu'il est vide.
    printf("The server doesn't know that file\n");
    remove(argv[3]);
  }
  
  close(sock);
  free(buffer);
  fclose(fileToReceive);
  return 0;
}
