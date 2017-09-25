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

#define BUFFERLIMIT 102                             // Include space for '/n' and '/0'


int main(int argc, char **argv, char**envp){
	
    char **myargs, **path;
    char **commandVec, *program;
    int rc, status, i;

    while(1){
        commandVec = waitForUserCommand();                // Get input command
        for(i=0; commandVec[i]; i++){
            myargs = tokenize(commandVec[i], ' ');                  // Generate command arguments vector
            if(myargs[0] != '\0'){                                  // empty commands should do nothing
                rc = fork();
                if (rc < 0) {                                  
                    fprintf(stderr, "Error: fork() failed\n");
                    exit(1);
                } 
                else if(rc == 0) {                                  // CHILD PROCESS 
                    execve(myargs[0], myargs, envp);                // Run command as it is ..     
                            
                    path = getPathEnvironment(envp);                // .. If previous execve returns, than check path environment
                    program = strconc("/", myargs[0]);
                    while(*path){
                        free(myargs[0]);
                        myargs[0] = strconc(*path, program);        // Run path/program        
                        execve(myargs[0], myargs, envp);
                        path++;
                    }
                    printf("Error: Command was not found \n");
                    exit(0);                                        // based on my point of view, a command not found should return 0
                } 
                else {                                              // parent goes down this path (original process)
                    status = 0;
                    int wc = wait(&status);
                    if(! WIFEXITED(status))
                        printf("Program does not terminate normally with exit or _exit \n");
                    else if(WEXITSTATUS(status) != 0)
                        printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
                }
            }
            freeVector(myargs);
        }
        freeVector(commandVec);
       
    }
    return 0;
	
}
/** Returns a vector in which each entry contains a command
 *  from the user
 */
char **waitForUserCommand(){    
    int len;
    char *str = (char *)malloc(BUFFERLIMIT);
    char **commandVec;
    
    write(2, "$ ", 2);
    len = read(0, str, BUFFERLIMIT);
    assert2(len < BUFFERLIMIT, "Limit of string length was overpassed");    
    
    if(len == 0)            // EOF also exits the program
        exit(0);
    
    str[len] = '\0';        // make sure there is a '\0' at the end
    
    if(strcomp(str, "exit\n")) 
        exit(0);
    
    commandVec = tokenize(str, '\n');        
    
    free(str);
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
/** Returns a vector contaning all the paths in the $PATH
 *  environment variable
 */
char **getPathEnvironment(char **envp){
    char **tenvp, **tokenVec, **pathVec;
    tenvp = envp;
    while(*tenvp){
        tokenVec = tokenize(*tenvp, '=');           // tokenize each envr variable to check before '='
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




    
