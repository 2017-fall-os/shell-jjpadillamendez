/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/********************************************************************/

#ifndef shell		// prevent multiple inclusion
#define shell

void freeVector(char **tokenVec);

char **waitForUserCommand();

char askForDelimit();

char **getPathEnvironment(char **envp);

#endif 
