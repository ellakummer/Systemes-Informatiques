#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

#define MaxConnectionsAttentes 256
#define MaxALire 256 
#define MaxChemin 1024
#define TBuffer 2048


/* Prépare l'adresse du serveur */
void prepare_address( struct sockaddr_in *address, int port) {
  size_t addrSize = sizeof( address );
  memset(address, 0, addrSize); // mettre tout à 0 
  address->sin_family = AF_INET; // IPv4
  address->sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY : pour affecter la socket à toutes les interfaces locales , htonl : obtenir adresse numérique valide
  address->sin_port = htons(port); // le port en big endian 
}

/* Construit le socket serveur */
int makeSocket( int port) {
  struct sockaddr_in address; // créer la structure (contient : famille, port, adresse internet)
  int sock = socket(PF_INET, SOCK_STREAM, 0); // créer descripteur du socket  (SOCKSTREAM -> par flot, avec connection) (0 car par TCP)
  if( sock < 0 ) {
    die("Failed to create socket");
  }
  prepare_address( &address, port );

  // on va attacher le socket serveur à une adresse : BIND 
  if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
  	die("Failed to bind the server socket");
  }

  // on va écouter le début d'une connection : LISTEN 
  if(listen(sock, MaxConnectionsAttentes) < 0) {
  	die("Failed to listen on server socket");
  }

  return sock;
}



/* lecture et écriture à partir du socket client (read/write) */
void echange( int ClientSocket, const char *chemin) {
  // il va falloir réceptionner ( <-> lire) le nombre de bytes demandés 
  int lect;
  int s = 0 ; // nombre de bytes déjà envoyés depuis buffer actuel
  int n; // nombres de bytes demandés par le client 
  lect = read(ClientSocket, &n ,MaxALire ); // lecture = réception du nombre de bytes 
  // ----------- TEST ------------------ 
  //printf("%d\n", n);
  //printf("%d\n", lect);

  if ( lect <= 0) {
  	die("Erreur de lecture");
  }
  int resteAEnvoyer= n; // la variable contiendra le nombre de bytes qui reste à envoyer ( utile s'il faut les envoyer en plusieurs fois à cause de la taille du buffer)
  // on va créer le buffer du serveur, avec 2048 nombres aléatoires à l'intérieur  
  unsigned char buffer[TBuffer];
  FILE *fp;
  fp = fopen("/dev/urandom", "r");
  fread(&buffer, 1, TBuffer, fp);
  while(resteAEnvoyer > 0) {
	  if ( resteAEnvoyer <= TBuffer - s) { // 2048-s = nombre de bytes restants dans buffer -> ok assez à envoyer
		// on va envoyer les bytes de Bs à Bs+resteaenvoyer
		unsigned char buffenvoy[n]; // ou on stock ceux que l'on va envoyer 
		for(int k=0; k < resteAEnvoyer; ++k) {
			buffenvoy[k] = buffer[s+k]; // (les s premiers du buffer déjà envoyés) 
		} 
		// on "l'écrit" au client (= les envoyer)
	  	write(ClientSocket,&buffenvoy,sizeof(buffenvoy));
		s = s + resteAEnvoyer; // on augmente le nombre de bytes du buffer utilisés 
		resteAEnvoyer = 0; // on a pu envoyer un nombre de bytes suffisants (condition du if)
	  } else if ( resteAEnvoyer > TBuffer - s) { // pas assez de nombre dans le buffer 
		// on va envoyer les bytes de Bs à B2048 (max pour ce coup ci)
		unsigned char buffenvoy[n];
		for(int k=0; k < TBuffer-s; ++k) {
			buffenvoy[k] = buffer[s+k];
		} 
	  	write(ClientSocket,&buffenvoy,sizeof(buffenvoy));
		resteAEnvoyer = resteAEnvoyer - (TBuffer - s);
		// puis on remplit le buffer
	  	fp = fopen("/dev/urandom", "r");
	  	fread(&buffer, 1, TBuffer, fp);
		s = 0; // le buffer de nouveau plein, aucun bytes utilises
      }
  }

  fclose(fp);
}



/* accepter une connexion et obtenir un socket client */ 

void DebutEchange(int ServeurSocket, const char *chemin) { // a besoin d'une socket et d'une adresse 
  while(1) {
  	struct sockaddr_in clientAdress; // structure IPV4 continent 3 champs : la famille, le port, l'adresse internte 
  	unsigned int clientLength = sizeof(clientAdress);
  	int clientSocket ;
	printf("En attente de connections\n");
	clientSocket = accept(ServeurSocket, (struct sockaddr *) &clientAdress, &clientLength);
	if (clientSocket < 0) {
		die("Failed to accept client connection");
	}

// lecture, écriture à partir du socket client : 
	printf("Client connected : %s\n", inet_ntoa(clientAdress.sin_addr)); // .sin_addr : on prend l'adresse ip en big endian de la structure adresse ---- inet_ntoa : converti l'adresse IPv4 en forme binaire 
	echange(clientSocket, chemin); 
  }
}




int main(int argc, char **argv) {
  int ServeurSocket;
  char* chemin;
  int port;

  if (argc != 2) {
  	exit(EXIT_FAILURE);
  }

  port = atoi(argv[1]); // converti un sting en entier (int) 

  // creer le socket 
  ServeurSocket = makeSocket(port);

  printf("Server running on port %d at dir '%s'\n", port, chemin);

  // on va s'occuper de l'échange ( voir les fonctions)
  DebutEchange(ServeurSocket,chemin);


  close(ServeurSocket); 






  return 0;
}
















