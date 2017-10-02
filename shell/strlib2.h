/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/********************************************************************/

#ifndef strlib2		// prevent multiple inclusion
#define strlib2

int countCharAt(char *str, char ch);

char *rmCharAt(char *str, char ch);

int strlen2(char *str);

int strcomp(char *str, char *str2);

char *strconc(char *str, char *str2);

char *copystr(char *str);

int isEmpty(char *str);

int addToEnvrVector(char **envr, char *var, char*value);

#endif 
