#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <dirent.h>


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

