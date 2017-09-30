/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/* This class implements a string library, which includes useful    */
/* procedures to manage strings                                     */
/********************************************************************/
#include "strlib2.h"
#include <stdlib.h>
#include <stdio.h>

int countCharAt(char *str, char ch){
    int count, i;
    
    count = 0;
    for(i=0; str[i]; i++){
        if(str[i] == ch){
            count++;
        }
    }
    
    return count;
    
}
char *rmCharAt(char *str, char ch){
    char *newstr;
    int i, j;
    for(i=0; str[i] && str[i] != ch; i++)
        ;
 
    if(str[i]){
        newstr = (char *)malloc(strlen2(str));
        for(j=0; j < i; j++){
            printf("%c \n", str[i]);
            newstr[j] = str[j];
        }
        for( ; str[j+1]; j++){
            newstr[j] = str[j+1];
        }
        newstr[j] = '\0';
    }else{
        newstr = copystr(str);
    }
    return newstr;
    
}
char *copystr(char *str){
    char *str2;
    int i;
    
    str2  = (char *)malloc(strlen2(str) + 1);
    for(i=0; str[i]; i++)
        str2[i] = str[i];
    str2[i] = '\0';
    
    return str2;
    
}
/* Return the number of characters in the given string
 */
int strlen2(char *str){
    char *tmpstr = str;
    while(*tmpstr != '\0')
        tmpstr++;
    return (tmpstr - str);          // since char is a byte       
    
}

/* Compares two strings and returns whether they are equal 
 * or not. [equal = 1, not equal = 0]
 */
int strcomp(char *str, char *str2){
    char areEqual = 1;
    if(strlen2(str) == strlen2(str2)){     // To be equal, must be of the same length
        char *tmpstr = str;
        char *tmpstr2 = str2;
        while(*tmpstr != '\0'){
            if(*tmpstr != *tmpstr2)        // Each char and its position must be equal
                areEqual = 0;
            tmpstr++;
            tmpstr2++;
        }
        
    }else{
        areEqual = 0;
    }
    return areEqual;
    
}
/** Returns a new freshly-allocated string which is the concatenation of
 *  the given two strings
 */
char *strconc(char *str, char *str2){
    int len, len2, i, j;
    char *newstr;
    
    len = strlen2(str);
    len2 = strlen2(str2);
    newstr = (char *)malloc(len + len2 + 1); 

    for(i=0; i < len; i++)
        newstr[i] = str[i];
    for(j=0; j < len2; j++)          
        newstr[i+j] = str2[j];         // i+j to overwrite no previous indexs    
    newstr[i+j] = '\0';
    
    return newstr;
    
}

int isEmpty(char *str){
    int empty, i;
    
    i = 0;
    empty = 1;
    
    if(str[i] != '\0'){
        for( ; str[i]; i++){
            if(str[i] != ' '){
                empty = 0;
                break;
            }
        }        
    }
    
    return empty;
    
}



























