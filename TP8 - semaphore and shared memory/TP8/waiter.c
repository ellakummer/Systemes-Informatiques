#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#include "common.h"
#include "waiter.h"

#define VITESSE_SERVEUSE 2

int main(){
  sersPizza();
}

void sersPizza(){

  int fd;
  int *SHMfd;

  // on ouvre la mémoire partagée créée par le cuisinier (donc nom déjà existant)
  if( (fd = shm_open("etagere", O_RDWR, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open etagere");
  }
  	// on lui assigme une adresse en mémoire virtuelle
  SHMfd = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if(SHMfd == MAP_FAILED){
    die("mmap");
  }
  	// on ouvre la sémaphore (créée aussi par le cuisinier)
  sem_t *semaphore = sem_open("/semTest", O_RDWR);
  if (semaphore == SEM_FAILED) {
    die("sem_open");
  }

  action(semaphore,SHMfd);

  printf("Le serveur a fini de servir les 1010 pizzas.\n");

  if(munmap(SHMfd, sizeof(int)) == -1){
    die("munmap");
  }

  	// on désassocie la mémoire partagée
  close(fd);
  if (sem_unlink("/semTest") < 0){
    perror("sem_close");
  }
  	// on détruit l'espace de mémoire partagée (le nom, puis le contenu associé à la région )
  shm_unlink("etagere");
  printf("Le serveur a fermé l'étagère et la semaphore.\n");
}

void action(sem_t *sem,int *SHMfd){
  int compteurPizza = 0;
  while(compteurPizza < 10){
    		// chaque pas de temps VITESSE_SERVEUSE, on verifiera s'il y a une pizza à servir sur l'étagère
    sleep(VITESSE_SERVEUSE);
    		// on bloque la sémaphore pour avoir l'accès exclusif et ne pas avoir de la concurrence sur l'étagère
    sem_wait(sem);
    if(*SHMfd > 0){
      			// s'il y a une(des) pizza(s) disponible (SHMfd : la mémoire partagée, un compteur qui équivaut au nombre de pizza sur l'étagère)
      *SHMfd = *SHMfd - 1; // on sert la pizza donc on décrémente le compteur de pizzas sur l'étagère
      printf("Pizza servie,nombre de pizza restante: %d\n",*SHMfd);
      compteurPizza++; // on incrémente le compteur de pizzas totales 
    }
    		// on libère la sémaphore, le cuisinier peut y avoir accès
    sem_post(sem);
  }
}
