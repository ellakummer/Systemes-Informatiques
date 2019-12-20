#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include "hashage.h"

// ON VA CONSIDÉRER L'ORDRE -f -t 

	#define TAILLE_MAX 100
	void LectureArg(int argc, char *argv[]) { // OBLIGÉ APPELER MAIN ?! 

	// MODULE 1 : 
	// on va lire les arguments passés en paramètres :
	// écrira le hasha de la chaine de caractère ou des fichiers passés en paramètres. 
	
	int sort;
	char hashvoulu[TAILLE_MAX];
	char* chaine1[TAILLE_MAX];
	bool dejaf = false; 
	bool dejat = false;
	bool fin = false;
	int deja;
	int longueurchaine = 0;
        // ON REGARDE JUSTE DE QUOI EST COMPOSEE NOTRE ENTREE : 
	while ((sort = getopt(argc, argv, "f:t:")) != -1) { // argc donne combien d'argument et argv[i] donne l'argument à la place i 
		switch (sort) {
			case 'f':
				dejaf = true ; // on note qu'on a eu une entrée de noms de fichiers 
				break ; 
			case 't' : 

				dejat = true; // on note qu'on a donné une fct de hachage en entrée 
				break;
	
			default: /* '?' */
				fin = true;
				break; 
		}
	}
	
	// -f -t sha f1 f1 
	if ((dejaf) && (dejat)) { // si on a une entre -f et -t
		chaine1[0] = argv[2];
		longueurchaine = longueurchaine + 1;
		for(int q = 5 ; q < argc ; q++) {
			chaine1[q-4] =  argv[q]; 
			longueurchaine = longueurchaine + 1; } 
		strncpy(hashvoulu, argv[4], TAILLE_MAX) ;
	} else if ((dejaf) && (!dejat)) { // donne un/des fichier(s) en entrée, mais pas de fonction de hachage 
		for(int r = 2 ; r < argc ; r++) { 
			chaine1[r-2] =  argv[r]; 
			longueurchaine = longueurchaine + 1; }
		strcpy(hashvoulu, "SHA1"); // si pas de -t précisé, SHA1 par défaut
	} else if ((dejat) && (!dejaf)) { // on donne une fonction de hashage mais pas de fichiers, donc une chaine de caractères
		for(int p = 3 ; p < argc ; p++) { 
			chaine1[p-3] =  argv[p]; 
			longueurchaine = longueurchaine + 1; }
		strncpy(hashvoulu, argv[2], TAILLE_MAX) ;
	} else { // on a une chaine de caractères en entrée 
		for(int p = 1 ; p < argc ; p++) { 
			chaine1[p-1] =  argv[p]; 
			longueurchaine = longueurchaine + 1; }
		strcpy(hashvoulu, "SHA1"); // si pas de -t précisé, SHA1 par défaut 
	}

	// TESTS SI NECESSAIRES 
	//printf("chaine=  ");
	//printf( "%s\n", chaine1[0]);
	//printf("chaine=  ");
	//printf( "%s\n", chaine1[1]);
	//printf("fct has voulue : ");
	//printf("%s\n", hashvoulu); // OK FONCTIONNE 

	// on change le booleen en int (plus facile pour argujment de la fonction hashage)
	if (dejaf) {
		deja = 1;
	} else {
		deja = 0;
	}
	hashage(hashvoulu, chaine1, deja, argc, argv, longueurchaine); // on hache ce qu'il faut 


	return;

}
