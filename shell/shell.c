/****************************************************/
/* Lab Assignment 1: Tokenizer   Date: 09/10/2017   */
/* Author: Jesus Padilla    Class: Operating System */
/* Prof: Dr. Freudenthal    TA: Adrian Veliz        */
/* This class implements a user interface used to   */
/* test a tokenizer program.                        */
/****************************************************/
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "assert2.h"
#include "mytoc.h"
#include "shell.h"
#include "strlib2.h"
#include <unistd.h>

#define BUFFERLIMIT 102                             // Include space for '/n' and '/0'

int main(int argc, char **argv, char**envp){
	
    char **myargs, **path;
    char *str, *program;
    int rc, status, retVal;
    
    while(1){
        str = waitForUserCommand();                // Get input command
        myargs = tokenize(str, ' ');               // Generate token vector
        
        if(myargs[0] != '\0'){
            rc = fork();
            if (rc < 0) {                                  
                fprintf(stderr, "Error: fork() failed\n");
                exit(1);
            } else if (rc == 0) {                        // Run child process
                path = getPathEnvironment(envp);           // what happens when the program end with path?
                program = strconc("/", myargs[0]);
                while(*path){
                    free(myargs[0]);
                    myargs[0] = strconc(*path, program);            // path/program
                    retVal = execve(myargs[0], myargs, envp);
                    path++;
                }
                exit(1); 
            } else {                                       // parent goes down this path (original process)
                status = 0;
                int wc = wait(&status);
                if(WIFEXITED(status))
                    printf("%d \n", WEXITSTATUS(status));
            }
        }
        free(str);                                
        freeVector(myargs);
    }
   
    return 0;
	
}
/** Returns an string which contains the input of the user
 */
char *waitForUserCommand(){    
    int len;
    char *str = (char *)malloc(BUFFERLIMIT);
    //char *ps = getenv("PS1");
    //write(1, ps, strlen2(ps)+1);                           // Read user input
    
    write(1, "$ ", 2);
    len = read(0, str, BUFFERLIMIT);
    assert2(len < BUFFERLIMIT, "Limit of string length was overpassed");              
    
    rmCharIn(str, '\n');                         // Remove new line char, replace it for '\0'
    str = (char *)realloc(str, len);
    
    if(strcomp(str, "exit"))
        exit(0);
        
    return str;
    
}
/** Free the given vector of strings. First, it frees each string
 *  then the empty vector.
 */
void freeVector(char **tokenVec){
    for(int i=0; tokenVec[i] != '\0'; i++){
        free(tokenVec[i]);
    }
    free(tokenVec);
    
}
char **getPathEnvironment(char **envp){
    char **tenvp, **tokenVec, **pathVec;
    tenvp = envp;
    while(*tenvp){
        tokenVec = tokenize(*tenvp, '=');
        if(strcomp(*tokenVec, "PATH")){
            pathVec = tokenize(tokenVec[1], ':');
            freeVector(tokenVec);
            break;
        }
        freeVector(tokenVec);
        tenvp++;
    }
    return pathVec;
    
}






    
