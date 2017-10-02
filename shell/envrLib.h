#ifndef envrLib
#define envrLib

char **getEnvrVar(char **envr, char *var);

char *getEnvrVar2(char **envr, char *var);

char *expandEnvrVar2(char **envr, char *myarg);

char **expandEnvrVar(char **envr, char **myargs);

int setEnvrVar(char **envr, char **myargs);

#endif
