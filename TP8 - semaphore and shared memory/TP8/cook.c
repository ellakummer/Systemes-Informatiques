#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#include "cook.h"
#include "common.h"

#define ATTENTE_MAX 2

int main(){
  sharedMemory();
}

void pizza(){
  	// "création" de la pizza: on va déterminer un temps de préparation aléatoire :
	// initialisation de srand sur le temps actuel
  srand(time(NULL));
  	// tps retourne un nombre aléatoire comprit entre 0 et RAND_MAX
  float tps = rand();
  tps = (tps/RAND_MAX)*ATTENTE_MAX+1;
  printf("Prépare la pizza pendant %f secondes\n",tps);
  	// on dort un certain temps = temps de préparation
  sleep(tps);
  printf("Pizza prête\n");
}

void sharedMemory(){
  	// création de la mémoire partagée puis utilisation avec: void action(sem_t *sem,int *SHMfd)
  int fd;
  int *SHMfd;

  // on créé "étagère" , notre espace mémoire partagé  (lecture-ecriture, erreur si nom déjà utilisé)
  if( (fd = shm_open("etagere", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open etagere");
  }
  	// on choisit la taille de la mémoire partagée : taille d'un entier
  if( ftruncate(fd, sizeof(int)) == -1){
    die("ftruncate");
  }
  	// on associe la mémoire partgée à l'espace virtuel du processus (mmap renvoie l'adresse)
  SHMfd = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if(SHMfd == MAP_FAILED){
    die("mmap etagere");
  }

  // on crée notre sémaphore
  sem_t *semaphore = sem_open("/semTest", O_CREAT | O_EXCL,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,1);
  if (semaphore == SEM_FAILED) {
    die("sem_open");
  }

  // on lance la fonction action() à qui on passe la sémaphore et la mémoire partagée)  (voir commentaires de la fonction)
  action(semaphore,SHMfd);

  printf("Le cuisinier a préparé 1010 pizzas, il rentre chez lui.\n");

  // supprime la projection en mémoire
  if(munmap(SHMfd, sizeof(int)) == -1){
    die("munmap SHMfd");
  }
  	// on désassocie la mémoire partagée
  close(fd);
  	// on ferme la sémaphore
  if (sem_close(semaphore) < 0){
    perror("sem_close");
  }
}

void action(sem_t *sem,int *SHMfd){
  int dodo = 0; // variable qui permettra de savoir si le cuisinier doit se reposer (3 pizzas sur l'étagère)
  int compteurPizza = 0;
  	// on veut préparer 10 pizzas
  while(compteurPizza < 10){
    if(dodo == 0){
      pizza();
      			// on fait une pizza donc incrémente le compteur
      compteurPizza++;
      			// on vérouille la sémaphore : accès pour le cuisinier, pas le serveur (éviter la concurrence)
      sem_wait(sem);
      			// on incrémente les pizzas qui sont aussi vues par le serveur, car dans la mémoire partagée
      *SHMfd = *SHMfd + 1;
      printf("Pizza mise dans l'étagère, nombre de pizza(s) dans l'etagère: %d\n",*SHMfd);
      if(*SHMfd == 3){
        			// si on a 3 pizzas, on va dire au cuisinier de reposer (dodo = 1)
        dodo = 1;
        printf("La cuisinière se repose\n");
      }
      sem_post(sem);
    }
    else{
      			// le cuisinier doit se reposer ( car dodo == 1)
      sleep(1);
      sem_wait(sem);
      if(*SHMfd <3){ // #pizzas < 3
        dodo = 0;
      }
      			// on débloque la sémaphore
      sem_post(sem);
    }
  }
}
