#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <dirent.h>
#include <time.h>


// AFFICHE LE CONTENU D'UNE SOURCE 
int ultracp(int argc, char source[]) {

    if (argc != 2) { // si on passe plus d'un argument (plus d'un fichier) 
        printf("trop d'arguments passés ");
        exit(EXIT_FAILURE);
    }

    int file=0;
    file = open(source, O_RDONLY);  
    if(file < -1) { // controlle d'erreur : on vérifier que l'assignation du int au fichier a bien été faite
    	return 1; }

    struct stat fileStat; // structure ou on va stocker fstat
    fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
    if(fstat(file,&fileStat) < 0) { // si la complétion n'a pas été bien faite 
    	return 1; }

    if (stat(source, &fileStat) == -1) {  
        perror("stat");
        exit(EXIT_FAILURE);} 

	// TYPE FICHIER : 

	    switch (fileStat.st_mode & S_IFMT) { 
	    case S_IFDIR:  printf("d");               break; // directory -> dossier ? 
	    case S_IFLNK:  printf("l");                 break; // lien symbolique
	    case S_IFREG:  printf("-");            break; // regular 
	    default:       printf("unknown?");                break;
	    }

	// DROITS DE LECTURE/ECRITURE/EXECUTIONS, POUR USER,GROUP,OTHER

	// read permission user
	    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
	// write permission user
	    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
	// execute user
	    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
	// read permission group
	    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
	// write permission group
	    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
	// execute permission group
	    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
	// read permission other
	    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
	// write permission other
	    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
	// execute permission other
	    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
	//   printf("\n\n");
	    printf("	"); 
	   
	// TAILLE FICHIER (bytes) :	
	   printf("%ld\t", fileStat.st_size);

	// DATE MODIFICATION 
	struct tm *time;
	char buffer[200];
	time = localtime(&fileStat.st_mtime);
	strftime(buffer, sizeof(buffer),"%c", time);
  	printf("%s\t", buffer);

	 // NOM DU FICHIER : 
	printf("%s\n", source);

	if (fileStat.st_mode & S_IFDIR) {

		struct dirent *de;
		DIR *dr = opendir(source); // on ouvre le repertoire que l'on veut copier 
		while((de = readdir(dr)) != NULL) {
			if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".")) { // vérifie qu'on ne boulce pas sur . ou .. sinon à l'infini mdr 
				    int file=0;
				    file = open(de->d_name, O_RDONLY);  
				    /*if(file < -1) { // controlle d'erreur : on vérifier que l'assignation du int au fichier a bien été faite
				    	return 1; }*/

				    struct stat fileStat; // structure ou on va stocker fstat
				    fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
				    /*if(fstat(file,&fileStat) < 0) { // si la complétion n'a pas été bien faite 
				    	return 1; }*/

				    /*if (stat(de->d_name, &fileStat) == -1) {  
					perror("stat");
					exit(EXIT_FAILURE);} */
					char temp[100];
					strcpy(temp, source);
				    ultracp(argc, strcat(strcat(temp,"/"),de->d_name));
	
			
			}
		}
	}
}




int modif42(char* dest[], char* source[]) {

	int file=0;
	file = open(dest, O_RDONLY);
	struct stat fileStat; // structure ou on va stocker fstat
	fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
	//struct tm *time;
	//char buffer[200];

	int filesource = 0;
	filesource = open(source, O_RDONLY);
	struct stat fileStatSource;
	fstat(filesource, &fileStatSource); 
	// ÇA FAISAIT TROP  MAL DE SUPPRIMER : 
	//struct tm *timesource;
	//char buffers[200];

		/*time = localtime(&fileStat.st_mtime);

		strftime(buffer, sizeof(buffer),"%y %j %H %M %S", time);
		timesource = localtime(&fileStatSource.st_mtime);
		strftime(buffers, sizeof(buffers),"%y %j %H %M %S", timesource);
			
		printf("temps destination : ");
		printf("%s\n", buffer);
		printf("temps source : ");
		printf("%s\n", buffers);

		char *token;
		token = strtok(buffer, " "); 
		char *token2;
		token2 = strtok(buffers, " "); 
		while(token != NULL) {
			
		for (int k = 0; k < 5; ++k) {
			if (token[k] < token[k]) {
				printf("source moins récente");
				break;
			} else if (token[k] > token[k]) {
				printf("source plus récente");
				break;
			} else if (k == 5) {
				printf("temps exact");
			}
			
		}*/

	if(fileStatSource.st_mtime < fileStat.st_mtime) {
		return 1; // source plus récente -> on va remplacer 
	} else if (fileStat.st_size != fileStatSource.st_size) { // fichiers de taille différente -> on va remplacer 
		return 1;
	} else {
		return 0; // source moins récente et même taille de fichier  -> on ne va pas remplacer
	}
}



// COPIE : source = fichiers, destination = fichier 
void copiefichfich(char *source,char *destination){
  FILE *fSource;
  FILE *fDest;
  char buffer[512];
  char temp[512];
  //int NbLus;
	
	 // vérification ouverture destination 
	if((fDest = fopen(destination,"w")) == NULL){
		perror("fopen");
		printf("%s\n",strcat(destination,source));
		fclose(fSource);
		exit(0);
	}

	// vérification ouverture du fichier source à copier 
	if((fSource = fopen(source,"r"))==NULL){ 
		perror("fopen");
		exit(0);
	}

	// copier du fichier : 
	char ch;
	while ((ch = fgetc(fSource)) != EOF) {
		fputc(ch, fDest);
	}
	
	//fermeture des fichiers 
	fclose(fDest);
	fclose(fSource);
}

//COPIE : source = fichier, destination = repertoire 
void copiefichrep(char *source,char *destination){
  FILE *fSource;
  FILE *fDest;
  char buffer[512];
  char temp[512];
  //int NbLus;

  strcpy(temp,destination);
	
	 // vérification ouverture destination 
	if((fDest = fopen(strcat(strcat(temp,"/"),source),"w")) == NULL){
		perror("fopen");
		printf("%s\n",strcat(strcat(temp,"/"),source));
		fclose(fSource);
		exit(0);
	}

	// vérification ouverture du fichier source à copier 
	if((fSource = fopen(source,"r"))==NULL){ 
		perror("fopen");
		exit(0);
	}


	// copier du fichier : 
	char ch;
	while ((ch = fgetc(fSource)) != EOF) {
		fputc(ch, fDest);
	}
	
	//fermeture des fichiers 
	fclose(fDest);
	fclose(fSource);
}

//COPIE : source = repertoire, destination = repertoire 
void copiereprep(char *source,char *destination){


	FILE *fSource;
	FILE *fDest;
	char buffer[512];
	char temp[512];

	//on en a besoin pour récupérer les droits du répertoire source
	int file=0;
	file = open(source, O_RDONLY); 
	struct stat fileStat; // structure ou on va stocker fstat
	fstat(file, &fileStat);

	// on va créer un  répertoire avec ce nom puis itérer à l'intérieur. 

	strcpy(temp,destination);

	strcat(strcat(temp,"/"),source);
	mkdir(temp, fileStat.st_mode);
	
}


// FONCTION QUI COPIE L'INTEREUR D'UN REPERTOIRE
void intRepertoire(char* source, char* destination) {
	struct dirent *de;
	DIR *dr = opendir(source); // on ouvre le repertoire que l'on veut copier 
	while((de = readdir(dr)) != NULL) {
		if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".")) { // vérifie qu'on ne boulce pas sur . ou ..
			if (de->d_type == DT_REG) { // si on y trouve un fichier
				char temp[100];
				char temp2[100];
				strcpy(temp2,destination);
				strcpy(temp, source);
				copiefichrep(strcat(strcat(temp,"/"),de->d_name),destination);
			}
			if(de->d_type == DT_DIR) { // si on y trouve un répertoire 
				// verifier qu'on a pas le même nom que 
				char temp[100];
				strcpy(temp,destination);
				// je veux récupérer les permissions de la source : 
				int file=0;
				file = open(source, O_RDONLY); 
				struct stat fileStat; 
				fstat(file, &fileStat);

				mkdir(strcat(strcat(strcat(strcat(temp,"/"),source),"/"),de->d_name),fileStat.st_mode); // on créer le répertoire, avec les mêmes permissions  
				char temp2[100];
				char temp3[100];
				strcpy(temp2,source);
				strcpy(temp3,destination);
				intRepertoire(strcat(strcat(temp2,"/"),de->d_name),destination); // on rappelle la fonction avec ce répertoire afin de copier son contenu dans la destination
			}
		}
	}
} 


// FONCTION QUI VA BOUCLER DANS LA SOURCE, AFIN DE COPIER UN À UN 
void boucle(int argc, char *argv[], char* dest[], int typedest) {
if (strcmp(argv[1], "-a") == 0) { //les droits seront modifiés dans tous les cas 
	for(int i=2;i<argc-1;i++){
		
		int typesource; // 1 si fichier. 0 si répertoire 
		char *source = argv[i];
		int file=0;
		file = open(source, O_RDONLY);
		struct stat fileStat; // structure ou on va stocker fstat
		fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
	
		if (fileStat.st_mode & S_IFDIR) { // répertoire 
			typesource = 0; 
		} else if (fileStat.st_mode & S_IFREG) { // fichier 
			typesource = 1;
		} 

		if (typedest == 0) { // destination = repertoire  

			if (typesource == 0) { // source = repertoire 
					// on va vérifier qu'il n'y ait pas déjà de repertoire à ce nom à l'intérieur : 
					struct dirent *de;
					DIR *dr = opendir(dest); // on ouvre le repertoire que l'on veut copier
					int verif = 0; 
					while((de = readdir(dr)) != NULL) {
						if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".") && strcmp(de->d_name, source)) { 
							if (modif42(de->d_name,source) == 0) {
								verif = verif + 1 ; // on note qu'il y a un nom similaire
								// on modifie ses droits car on a -a 
								int file2=0;
								file = open(de->d_name, O_RDONLY);
								struct stat fileStatDest; 
								fstat(de->d_name, &fileStatDest);
								fileStatDest.st_mode = fileStat.st_mode;	
							} else {
								remove(de); // on supprime celui au même nom car on doit modifié (vu avec modif42)
							}
						} 	
					}
					if (verif == 0) { // pas de nom similaire : ok on copie 
						copiereprep(source,dest);
						// BOUCLER DANS LE RÉPERTOIRE 
						intRepertoire(source,dest);
					}
  
			} else { // source = fichier
				// on va vérifier qu'il n'y ait pas déjà de fichier à ce nom : 
				struct dirent *de;
				DIR *dr = opendir(dest); // on ouvre le repertoire que l'on veut copier
				int verif = 0; 
				while((de = readdir(dr)) != NULL) {
					if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".") && strcmp(de->d_name, source)) { // vérifie qu'on ne boulce pas sur . ou .., et s'il y a un nomsimilaire 
						if (modif42(de->d_name,source) == 0) {
							verif = verif + 1 ; // on note qu'il y a un nom similaire
							// on va remplacer les droits car on a eu -a 
							int file2=0;
							file = open(de->d_name, O_RDONLY);
							struct stat fileStatDest; // structure ou on va stocker fstat
							fstat(de->d_name, &fileStatDest);
							fileStatDest.st_mode = fileStat.st_mode;
						} else {
							remove(de); // on supprime celui au même nom 
						}
					} 	
				}
				if (verif == 0) { // pas de nom similaire : ok on copie 
					copiefichrep(source,dest);
				}
			}
		} else if (typedest == 1) { // destination = fichier 
			if (typesource == 0) { // source = repertoire 
				printf("erreur : ne peut pas copier un répertoire dans un fichier");
			} else { // source = fichier
				// vérifier qu'il y a qu'un fichier à copier : (1 fichier -> argc = 4 (appel+-a+fich+dest))
				if (argc != 4) {
					printf("pas le bon nombre d'arguments : pour une destination de type fichier, il faut UNE source");
				} else { 
					if (strcmp(source,dest) != 0) { // si nom source différente de destination 
						copiefichfich(source,dest);
					} else { // si le nom de la source est égal à celui de la destination 
						if (modif42(dest,source)) { // si on doit modifier (point 4.2) ( sinon on ne fera rien : pas de else) 
							copiefichfich(source,dest);
						} else { // on a pas modifié celuid déjà existant mais on lui modifie ses droits car on a eu -a 
							int file2=0;
							file = open(dest, O_RDONLY);
							struct stat fileStatDest; 
							fstat(dest, &fileStatDest);
							fileStatDest.st_mode = fileStat.st_mode;
						}
					}
				}
			 }	
		}
	}

} else {


	for(int i=1;i<argc-1;i++){
		
		int typesource; // 1 si fichier. 0 si répertoire 
		char *source = argv[i];
		int file=0;
		file = open(source, O_RDONLY);
		struct stat fileStat; // structure ou on va stocker fstat
		fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
	
		if (fileStat.st_mode & S_IFDIR) { // répertoire 
			typesource = 0; 
		} else if (fileStat.st_mode & S_IFREG) { // fichier 
			typesource = 1;
		} 

		if (typedest == 0) { // destination = repertoire 
			if (typesource == 0) { // source = repertoire 
					// on va vérifier qu'il n'y ait pas déjà de repertoire à ce nom à l'intérieur : 
					struct dirent *de;
					DIR *dr = opendir(dest); // on ouvre le repertoire que l'on veut copier
					int verif = 0; 
					while((de = readdir(dr)) != NULL) {
						if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".") && strcmp(de->d_name, source)) { 
							if (modif42(de->d_name,source) == 0) {
								verif = verif + 1 ; // on note qu'il y a un nom similaire
							} else {
								remove(de); // on supprime celui au même nom 
							}
						} 	
					}
					if (verif == 0) { // pas de nom similaire : ok on copie 
						copiereprep(source,dest);
						// BOUCLER DANS LE RÉPERTOIRE 
						intRepertoire(source,dest);
					}

			} else { // source = fichier
				// on va vérifier qu'il n'y ait pas déjà de fichier à ce nom : 
				struct dirent *de;
				DIR *dr = opendir(dest); // on ouvre le repertoire que l'on veut copier
				int verif = 0; 
				while((de = readdir(dr)) != NULL) {
					if (strcmp(de->d_name, "..") && strcmp(de->d_name, ".") && strcmp(de->d_name, source)) { // vérifie qu'on ne boulce pas sur . ou .., et s'il y a un nomsimilaire 
						if (modif42(de->d_name,source) == 0) {
							verif = verif + 1 ; // on note qu'il y a un nom similaire
						} else {
							remove(de); // on supprime celui au même nom 
						}
					} 	
				}
				if (verif == 0) { // pas de nom similaire : ok on copie 
					copiefichrep(source,dest);
				}
			}
		} else if (typedest == 1) { // destination = fichier 
			if (typesource == 0) { // source = repertoire 
				printf("erreur : ne peut pas copier un répertoire dans un fichier");
			} else { // source = fichier
				// vérifier qu'il y a bien qu'un fichier à copier 
				if (argc != 3) {
					printf("pas le bon nombre d'arguments : pour une destination de type fichier, il faut UNE source");
				} else {
					if (strcmp(source,dest) != 0) { // si nom source différente de destination 
						copiefichfich(source,dest);
					} else { // si le nom de la source est égal à celui de la destination 
						if (modif42(dest,source)) { // si on doit modifier (point 4.2) ( sinon on ne fera rien : pas de else) 
							copiefichfich(source,dest);
						}
					}
				}
			}
		}
	}
}
}



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {

//-----------------------------

	// ON S'OCCUPE DE LA DESTINATION :

	int typedest; // 1 si fichier. 0 si répertoire 
	char *dest = argv[argc-1];
	int file=0;
	file = open(dest, O_RDONLY);
	
	struct stat fileStat; // structure ou on va stocker fstat
	fstat(file, &fileStat); // fstat renverra des informations sur un fichier : ici file 
	
	// VERIFIER SI LE NOM DEMANDÉ EST UN RÉPERTOIRE : 
	if (fileStat.st_mode & S_IFDIR) { 
		typedest = 0; 
	// VERIFIER SI EST UN FICHIER : 
	} else if (fileStat.st_mode & S_IFREG) { 
		typedest = 1;
	} else {
	// SINON CREER UN FICHIER AU NOM DEMANDÉ : 
		FILE *fp = NULL;
		fp = fopen(dest, "a"); // récrire 
		typedest = 1; 
	}


//----------------------------

	// ON VA ITÉRER DANS LA BOUCLE
	boucle(argc,argv, dest, typedest);

// --------------------------

	// ON AFFICHE LA DESTINATION APRÈS LES COPIES 
	ultracp(2,dest); 

// ----------------------


return 0;
}
