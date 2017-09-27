/********************************************************************/
/* Shell: Lab Assignment 2, Part 1         Date: 09/10/2017         */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/* This program implements a shell which executes simple commands   */
/********************************************************************/
#include <sys/types.h>
#include <unistd.h> 		// for pipe
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "assert2.h"
#include "mytoc.h"
#include "shell.h"
#include "strlib2.h"

#define BUFFERLIMIT 102                             // Include space for '/n' and '/0'
#define NEXT 1
#define PREV 0

int main(int argc, char **argv, char**envp){
	
    char **myargs, **path, **pipeVec;
    char **commandVec, *program;
    int **pipeFds;
    int rc, status, i, j, pipeLen;

    while(1){
        commandVec = waitForUserCommand();                // Get input command
        for(i=0; commandVec[i]; i++){
            pipeVec = tokenize(commandVec[i], '|');
            pipeLen = vectorLength(pipeVec);
            pipeFds = initPipe(pipeLen);
            
            for(j=0; j < pipeLen; j++){
                myargs = tokenize(pipeVec[j], ' '); 
                
                if(myargs[0] != 0){                          // empty commands should do nothing
                    if(j > 0 && pipeLen > 1){
                        pipeFds[PREV] = pipeFds[NEXT];
                        if(j < pipeLen - 1){
                            pipeFds[NEXT] = (int *)calloc(2, sizeof(int));
                            pipe(pipeFds[NEXT]);
                        }
                    }
                    rc = fork();
                    if (rc < 0) {                                  
                        fprintf(stderr, "Error: fork() failed\n");
                        exit(1);
                    } 
                    else if(rc == 0) {                                  // CHILD PROCESS
                        updatePipe(pipeFds, j, pipeLen);                // Update file descriptor connecting to pipe
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
                        int wc = waitpid(rc, &status, 0);
                        if(! WIFEXITED(status))
                            printf("Program does not terminate normally with exit or _exit \n");
                        else if(WEXITSTATUS(status) != 0)
                            printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
                    }
                }
                freeVector(myargs);
            }
        }
        freeVector(commandVec);
       
    }
    return 0;
	
}
void updatePipe(int **pipeFds, int pipeNum){
    if(pipeFds){
        if(pipeNum > 0){
            close(0);                                   // close standard input
            dup(pipeFds[PREV]);
            closePipe(pipeFds[PREV]);
        }
        if(pipeNum < pipeLen - 1){
            close(1);                                   // close standar output
            dup(pipeFds[NEXT]);
            closePipe(pipeFds[NEXT]);
        }
    
    }

}
int **initPipe(int pipeLen){
    if(pipeLen > 1){
        int retVal;
        int **pipeFds; 
        
        pipeFds = (int **)calloc(3, sizeof(int *));
        pipeFds[NEXT] = (int *)calloc(2, sizeof(int));
        retVal = pipe(pipeFds[NEXT]);
        
        if(retVal == -1){
            char *msg = "Error: Pipe system call failed";
            write(2, msg, strlen2(msg));
            exit(0);
        }
        return pipeFds;
        
    }
    return 0;
    
}
void closePipe(int *pipeFds){
     close(pipeFds[0]); 
     close(pipeFds[1]);
     free(pipeFds);
     
}
/** Returns a vector in which each entry contains a command
 *  from the user
 */
char **waitForUserCommand(){    
    int len;
    char *str = (char *)malloc(BUFFERLIMIT);
    char **commandVec;
    
    write(1, "$ ", 2);
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
int vectorLength(char **vector){
    int len, i;
    len=0;
    for(i=0; vector[i]; i++){
        len++;
    }
    return len;
    
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




    
