int hdlrSetup(int sig, void (*hdlr)(int), int flags);

void INThdlr (int sig);
void HUPhdlr (int sig);
void CHLDhdlr(int sig);

void  setMainProc(pid_t pid);
pid_t getMainProc(void);
void  setBgProc  (pid_t pid);
pid_t getBgProc  (void);
