#ifndef envrLib
#define envrLib

char **getEnvrVar(char **envr, char *var);

char *getEnvrVar2(char **envr, char *var);

char *expandEnvrVar(char **envr, char *myarg);

int setEnvrVar(char **envr, char **myargs);

#endif
