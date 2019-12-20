#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define TAILLE_MAX 100 
#define MAX_ARG 10 


/*


WEXITSTATUS(status)
    renvoie le code de sortie du fils. Ce code est constitué par les 8 bits de poids faibles de l'argument status que le fils a fourni à exit(3) ou à _exit(2) ou l'argument d'une commande de retour dans main(). Cette macro ne peut être évaluée que si WIFEXITED a renvoyé vrai. 


*/

pid_t pidEnfant = 0;
struct sigaction oldAction;

static void deadKnown(int sig, siginfo_t *siginfo, void *context){
	int status;
	pid_t pid;
	pid = waitpid(pidEnfant,&status,0);
	int exit_status = WEXITSTATUS(status);
	printf("Background job %d exited with code %d\n",pid,exit_status);
	if (sigaction(SIGCHLD, &oldAction, NULL) < 0) {
		perror("Sigaction");
	}
}


void jobs(int argc, char *argv[]) {

	int statut;
    	pid_t w,pid;
	int wstatus;
	pid = fork();

	// tache de fond : 
	char str[MAX_ARG];
	strcpy(str,"&");
	char **argv2 = NULL; 
	argv2 = malloc(sizeof(char *) * MAX_ARG);
	// SI ON A & À LA FIN, ON VA LANCER EN TACHE DE FOND 
	if (strcmp(argv[argc-1],str) == 0) {
		// on va créer un nouveau argv sans le & de fin 
		for (int k = 0; k < argc-1; ++k) {
			argv2[k] = argv[k];
		}

		if (pid > 0) { 
			printf("pid child: %d\n", pid);
			// code parent 
			// pas de wait car on ne veut pas attendre qu'il se finisse, on veut lancer en tache de fond et direct redonner la main  
			// handler, gestion signal : 
			struct sigaction sa;
			sa.sa_sigaction = deadKnown;
			sigset_t ss;
			sigemptyset(&ss); // pas de masque
			sa.sa_mask = ss;
			sa.sa_flags = SA_SIGINFO; // pas de flags  
			sleep(1);
			sigaction(SIGCHLD, &sa, NULL);  

		} else if (pid == 0) {  
			// code enfant
			//printf("alpha\n");
			char chemin[TAILLE_MAX];
			strcpy(chemin, "/bin/");
			strcat(chemin,argv2[0]);
			freopen("/dev/null","w",stdin); // si stdout : tester en remplaçant /dev/null par un fichier.txt -> écrira dedans
			execv(chemin,argv2);
			exit(EXIT_SUCCESS);


		} else {
			perror("Error"); // le fork n'a pas fonctionné 
			exit(EXIT_FAILURE);
			}

	} else {
	// SINON LANCE EN NORMAL EN ENTREE/SORTIE STANDART 
		if (pid > 0) { 
			sleep(1);
		// code parent  
			//pid_t sortie = wait(pid);     
			do {
				w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED); // revenir si un fils est bloqué , revenir si un fils bloqué a été relancé par la délivrance du signal SIGCONT. 
				if (w == -1) {
					perror("waitpid");
					exit(EXIT_FAILURE);
				}

				if (WIFEXITED(wstatus)) { // renvoie vrai si le fils s'est terminé normalement
					printf("exited, status=%d\n", WEXITSTATUS(wstatus));
		           	} else if (WIFSIGNALED(wstatus)) { //renvoie vrai si le fils s'est terminé à cause d'un signal. 
		              		printf("killed by signal %d\n", WTERMSIG(wstatus));
		           	} else if (WIFSTOPPED(wstatus)) { // renvoie vrai si le fils a été arrêté par la délivrance d'un signal
		               		printf("stopped by signal %d\n", WSTOPSIG(wstatus));
		           	} else if (WIFCONTINUED(wstatus)) { //  renvoie vrai si le processus fils a été relancé par la délivrance du signal SIGCONT. 
		               		printf("continued\n");
		           	}
		       } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus)); // terminé normalement ou terminé à cause signal 
		                                                               

	    	} else if (pid == 0) {  
		// code enfant
			char chemin[TAILLE_MAX];
			strcpy(chemin, "/bin/");
			strcat(chemin,argv[0]);
			execv(chemin,argv);
			exit(EXIT_SUCCESS);


		} else {
			perror("Error"); // le fork n'a pas fonctionné 
			exit(EXIT_FAILURE);
		}
	}

}

void builtin(int argc, char *argv[]) { 
	char str1[MAX_ARG];
	char str2[MAX_ARG];
	strcpy(str1,"cd");
	strcpy(str2,"exit");
	if (strcmp(argv[0], str1) == 0) {
		// on va faire cd  avec la fonction ChangeDir : int chdir(const char *path);
		printf("%s\n", argv[1]);
		printf("on va changer de directory\n");
		int ret;
		ret = chdir (argv[1]);
		if (ret == -1) {
			perror("la redirection n'a pas fonctionné");
		}
		//mkdir("lala",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // TEST 
	}
	if (strcmp(argv[0], str2) == 0) {
		// on va faire exit 
		//printf("on va exit"); // test 
		exit(EXIT_SUCCESS);
	}
}


int main() {
while(1) {
	// on va récupérer une commande entrée (= chaine de caractère) : 
	char commande[TAILLE_MAX];
	fgets(commande, 100, stdin);
	//printf("%s\n", commande); // TEST !! 

	// transformer la chaine de caractère en un tableau de chaine au format argv: 
	int argc = 0; 
	size_t i = 0; // servira de compteur 

	char **argv = NULL;
	argv = malloc(sizeof(char *) * MAX_ARG);

	char *token;
	token = strtok(commande, " \n");
	while (token != NULL ) {
		argc = argc + 1 ; // à chaque mot lu , argc est incrémenté 
		argv[i] = malloc(sizeof(char) * (1+strlen(token)));
		strcpy(argv[i], token); // on ajoute le mot à argv 
		i++;
		token = strtok(NULL, " \n");
	}
/*
	// TEST ARGC : 
	printf("%d\n",argc); 

	// TESTS ARGV : 
	for(i = 0; argv[i] != NULL; i++)
	{
		printf("arg[%d] = '%s' ", i, argv[i]);
	}

*/

	// GESTION DES SIGNAUX :
	// SIGQUIT ET SIGTERM ignorés : 
	signal(SIGQUIT, SIG_IGN); // on ignore SIGQUIT
	signal(SIGTERM, SIG_IGN); // on ignore SIGTERM
	//raise(SIGQUIT); // test envoie signal  : est ignoré 
	//raise(SIGTERM); // test envoie signal : est ignoré 
	
	// À FAIRE BISOUS BONNE CHANCE AHHA 
	// SIGNINT
	struct sigaction sa,so;
	sa.sa_sigaction = deadKnown;
	sigset_t ss;
	sigemptyset(&ss); // pas de masque
	sa.sa_mask = ss;
	sa.sa_flags = SA_SIGINFO; 
	sleep(1);
	sigaction(SIGINT, &sa, NULL);  
	// SIGHUP 
	so.sa_sigaction = deadKnown;
	so.sa_mask = ss;
	so.sa_flags = SA_SIGINFO;   
	sleep(1);
	sigaction(SIGHUP, &sa, NULL); 
	
	char str1[MAX_ARG];
	char str2[MAX_ARG];
	strcpy(str1,"cd");
	strcpy(str2,"exit");
	// APPEL À BUILTIN SI CD OU EXIT 
	if ((strcmp(argv[0],str1) == 0) || (strcmp(argv[0],str2)) == 0){
		builtin(argc,argv);
	} else {
	// APPEL À JOB SINON 
		jobs(argc,argv);
	}

	// On libère le pointeur (sinon gros bordel c'était fun) 
	for (int j = 0; j < argc; j++) {
		free(argv[j]);
	}
}

	exit(EXIT_SUCCESS);
}
