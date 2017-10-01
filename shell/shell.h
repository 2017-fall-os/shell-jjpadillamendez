/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/********************************************************************/

#ifndef shell		// prevent multiple inclusion
#define shell

char **waitForUserCommand();

char **getPathEnvironment(char **envp);

int redirectInput(char **myargs);

int redirectOutput(char **myargs);

char **tokenizeAndCheckSyntax(char *str, char delimer, int syntax_case);

void executeChild(char **myargs, char **envp);

int fork2();

int isChangeDirRequested(char **myargs);

#endif 
