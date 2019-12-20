#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <stdbool.h>

#define TAILLE_MAX 10000

void hashage(char hashvoulu[TAILLE_MAX], char* chaine1[TAILLE_MAX], int dejaf, int argc, char *argv[], int longueurchaine) { 
// hashvoulu = fonction de hachage à utiliser, chaine1 contient les nom des fichiers ou la chaine de caractère, dejaf=1 si ce sont des noms de fichiers et = 0 si chaine1 est une chaine de carcatères
// longueur chaine donne combien il y a d'éléments dans chaine 1, utile si fichiers


	OpenSSL_add_all_digests();
        
	EVP_MD_CTX *mdctx;
        const EVP_MD *md;
        unsigned char md_value[EVP_MAX_MD_SIZE];
        int md_len, i;


	// on regarde si on va utiliser des fichiers entrés en argument:
	if (dejaf == 1) { 
		FILE* fichier = NULL; // initialise le fichier
	md = EVP_get_digestbyname(hashvoulu);
        if(!md) { // si la fonction de hachage demandée n'est pas une fonction de hachage
               printf("Unknown message digest %s\n", argv[1]);
               return;
        }


	mdctx = EVP_MD_CTX_create(); 
	EVP_DigestInit_ex(mdctx, md, NULL);
		for(size_t j = 0 ; j < longueurchaine ; j++) { // on itère sur tous les fichiers passés en argument
			char interne[TAILLE_MAX]; // variable afin de stocker l'interieur du fichier 
			fichier = fopen(chaine1[j], "r"); // on ouvre le fichier 
			while(fgets(interne, TAILLE_MAX, fichier) != NULL ){

				EVP_DigestUpdate(mdctx, interne, strlen(interne)); 

				EVP_DigestFinal_ex(mdctx, md_value, &md_len);
				printf("\n");
				printf("Digest is: ");
				for(i = 0; i < md_len; i++) {
					printf("%02x", md_value[i]); }
				printf("\n");
			}
		
	}
	EVP_MD_CTX_destroy(mdctx);
	EVP_cleanup();
	} else {
		// on va crypter la chaine de caractère que l'on a gardée dans la variable chaine1 

		// procédure de cryptage
		md = EVP_get_digestbyname(hashvoulu);
		mdctx = EVP_MD_CTX_create();
		EVP_DigestInit_ex(mdctx, md, NULL);
		EVP_DigestUpdate(mdctx, chaine1, strlen(chaine1));
        	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
        	EVP_MD_CTX_destroy(mdctx);

		printf("Digest is: ");
		for(i = 0; i < md_len; i++) {
			printf("%02x", md_value[i]); }
		printf("\n");

		EVP_cleanup();
	}
	

        EVP_cleanup();
        return;

}


