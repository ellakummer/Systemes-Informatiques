#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include "hashage.h"
#include "LectureArg.h"

// ON VA CONSIDÉRER L'ORDRE -f f1.. -t fcthash 

	#define TAILLE_MAX 100
	int main(int argc, char *argv[]) { 
	LectureArg(argc, argv); // on appelle la fonction qui lit les arguments entrés et retourne grace à la fonction hachage, le digest de soit la chaine de carctères, soir des fichiers.
	return 0;

} 
