/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/********************************************************************/

#ifndef mytoc		// prevent multiple inclusion
#define mytoc


char **tokenize(char *str, char delim);

char **tokenize2(char *str, char delim);

char *headAfter(char *str, char delim);

int countToks(char *str, char delim);

int countToks2(char *str, char delim);

int countTokLen(char *str, char delim);


#endif 
