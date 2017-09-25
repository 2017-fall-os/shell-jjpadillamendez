/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/* This program implements a shell which executes simple commands   */
/********************************************************************/
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
    char **commandVec, *program;
    int rc, status, i;

    while(1){
        commandVec = waitForUserCommand();                // Get input command
        for(i=0; commandVec[i]; i++){
            myargs = tokenize(commandVec[i], ' ');                  // Generate token vector
            if(myargs[0] != '\0'){                                  // empty commands should do nothing
                rc = fork();
                if (rc < 0) {                                  
                    fprintf(stderr, "Error: fork() failed\n");
                    exit(1);
                } 
                else if(rc == 0) {                                  // child process
                    execve(myargs[0], myargs, envp);                // Run command as it was inputted     
                            
                    path = getPathEnvironment(envp);                // If execve returns, than check path environment
                    program = strconc("/", myargs[0]);
                    while(*path){
                        free(myargs[0]);
                        myargs[0] = strconc(*path, program);        // Run path/program        printf("%s \n", myargs[0]);
                        execve(myargs[0], myargs, envp);
                        path++;
                    }
                    printf("Error: Command was not found \n");
                    exit(0); 
                } else {                                       // parent goes down this path (original process)
                    status = 0;
                    int wc = wait(&status);
                    if(! WIFEXITED(status))
                        printf("Program does not terminate normally with exit or _exit \n");
                    else if(WEXITSTATUS(status) != 0)
                        printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
                }
            }
        }
        freeVector(commandVec);
        freeVector(myargs);
    }
   
    return 0;
	
}
/** Returns an string which contains the input of the user
 */
char **waitForUserCommand(){    
    int len;
    char *str = (char *)malloc(BUFFERLIMIT);
    char **commandVec;
    
    write(2, "$ ", 2);
    len = read(0, str, BUFFERLIMIT);
    assert2(len < BUFFERLIMIT, "Limit of string length was overpassed");    
    
    if(len == 0) 
        exit(0);
    
    str = (char *)realloc(str, len+1);

    if(strcomp(str, "exit\n")) 
        exit(0);
    
    commandVec = tokenize(str, '\n');
    
    return commandVec;
    
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




    
