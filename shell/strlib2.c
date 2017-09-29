/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/* This class implements a string library, which includes useful    */
/* procedures to manage strings                                     */
/********************************************************************/
#include "strlib2.h"
#include <stdlib.h>

char *rmCharAt(char *str, char ch){
    char *newstr;
    for(int i=0; str[i]; i++){
        if(str[i] == ch){
            newstr = (char *)malloc(strlen2(str));
            for(int j=0; str[j]; j++){
                if(j != i)
                    newstr[j] = str[j];
            }
            newstr[j] = '\0';
            break;
        }
    }
    
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



























