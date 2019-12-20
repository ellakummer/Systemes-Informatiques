#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
Le pid du processus principal;
une valeur de 0 indique pas de processus principal en cours.
*/
static pid_t mainProcPid;
void setMainProc(pid_t pid) {
	mainProcPid = pid;
}
pid_t getMainProc(void) {
	return mainProcPid;
}

/*
Le pid du processus de fond;
une valeur de 0 indique pas de processus de fond en cours.
*/
static pid_t bgProcPid;
void setBgProc(pid_t pid) {
	bgProcPid = pid;
}
pid_t getBgProc(void) {
	return bgProcPid;
}


/* S'occupe d'un zombie. Si ifNoHang != 0, waitpid sera non-bloquant. */
static pid_t reap(int ifNoHang) {
	pid_t cpid;
	int   status, temp, isMainProc;

	temp = errno;
	cpid = waitpid(-1, &status,
		(ifNoHang ? WNOHANG : 0) | WUNTRACED | WCONTINUED);
	if (cpid == -1) {
		perror("Error waiting for process");
		goto exit;
	}
	if (cpid == 0)
		goto exit;
	
	isMainProc = cpid == mainProcPid;
	printf("%s job %d ", isMainProc ? "Main" : "Background", cpid);
	if (WIFEXITED(status)) {
		printf("exited normally with code %d\n", WEXITSTATUS(status));
		goto clearProc;
	}
	if (WIFSIGNALED(status)) {
		printf("terminated by signal %d%s\n", WTERMSIG(status),
			#ifdef WCOREDUMP
				WCOREDUMP(status) ? "; core dumped" : ""
			#endif
		);
		goto clearProc;
	}
	if (WIFSTOPPED(status)) {
		printf("stopped by signal %d\n", WSTOPSIG(status));
		goto exit;
	}
	if (WIFCONTINUED(status)) {
		printf("resumed by delivery of SIGCONT\n");
		goto exit;
	}

clearProc:
	if (isMainProc)
		setMainProc(0);
	else
		setBgProc(0);
exit:
	errno = temp;
	return cpid;
}

// S'occupe de SIGCHLD.
void CHLDhdlr(int sig) {
	reap(1);
}

// Envoie le signal sig au processus pid si pid != 0 .
static int killProc(int pid, int sig) {
	if (pid && kill(pid, sig))
		return -1;
}

// S'occupe de SIGINT.
void INThdlr(int sig) {
printf("Sent SIGINT to main proces.\n");
	if (killProc(mainProcPid, SIGINT))
		perror("Error sending SIGINT to main process");
}

// S'occupe de SIGHUP.
void HUPhdlr(int sig) {
printf("Sent SIGHUP to both processes.\n");
	if (killProc(mainProcPid, SIGHUP))
		perror("Error sending SIGHUP to main process");
	if (killProc(bgProcPid,   SIGHUP))
		perror("Error sending SIGHUP to background process");
	// EXIT LE SHELL?
}


// Assigne le handler hdlr au signal sig, avec flags en options.
// Ajoute des options par défaut, notamment SA_RESTART.
int hdlrSetup(int sig, void (*hdlr)(int), int flags) {
	sigset_t ss;
	sigemptyset(&ss);
	struct sigaction sa = {
		.sa_handler = hdlr,
		.sa_mask    = ss,
		.sa_flags   = flags | SA_RESTART
	};
	
	if (sigaction(sig, &sa, NULL)) {
		perror("Error assigning handler");
		return -1;
	}
	return 0;
}
