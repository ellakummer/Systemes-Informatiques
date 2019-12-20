#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

// Exécute le programme spécifié par argv (supposé dans "/bin"),
// et redirige son stdin vers /dev/null si ifBg != 0 .
void execute(char *argv[], int ifBg) {
	char chemin[PATH_MAX] = "";

	if (ifBg && !freopen("/dev/null", "w", stdin)) {
		perror("Error redirecting stdout in child");
		exit(EXIT_FAILURE);
	}
	strcat(chemin, "/bin/");
	strcat(chemin, argv[0]);
	execv(chemin, argv);
}
