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
#include <fcntl.h>
#include "assert2.h"
#include "mytoc.h"
#include "shell.h"
#include "strlib2.h"
#include "pipeLib.h"

#define BUFFERLIMIT 102                             // Include space for '/n' and '/0'
#define STDOUT 1
#define STDIN  0

int main(int argc, char **argv, char**envp){
	  
    char **myargs, **path, **pipeVec;
    char **commandVec, *program, **bgTaskVec, **redirectVec;
    int rc, status, pipeLen, bgTaskLen;
    int *pfds_prev, *pfds_next;
    int bgChild;
    
    while(1){
        commandVec = waitForUserCommand();                // Get input command
        for(int i=0; commandVec[i]; i++){
            bgTaskVec = tokenize(strconc(commandVec[i], " "), '&');
            bgTaskLen = vectorLength(bgTaskVec);
            for(int k=0; bgTaskVec[k]; k++){
                pipeVec = tokenize(bgTaskVec[k], '|');
                pipeLen = vectorLength(pipeVec);
                for(int j=0; pipeVec[j]; j++){
                    myargs = tokenize(pipeVec[j], ' ');                    // Generate command arguments vector
                    if(myargs[0] != '\0'){                                 // empty commands should do nothing
                        if(pipeLen > 1){
                            if(j != 0){
                                pfds_prev = pfds_next;                             // ready to take input from pipe
                            }
                            if(j != pipeLen-1){                                 // syntax error when pipelen > 1 and myargs[0]
                                pfds_next = (int *)calloc(2, sizeof(int));
                                pipe(pfds_next);
                            }
                        }
                        rc = fork();
                        if (rc < 0) {                                  
                            fprintf(stderr, "Error: fork() failed\n");
                            exit(1);
                        } 
                        else if(rc == 0) {                                  // CHILD PROCESS 
                            // Redirect input to a text file if required
                            if(!redirectInput(myargs)){
                                connectToInputPipe(pfds_prev, j, pipeLen);         // connect to pipe input otherwise if required
                            }else{
                                myargs = removeFromVectorAfter(myargs, 1);
                                closeInputPipe(pfds_prev, j, pipeLen);
                            }
                            // Redirect output to a text file if required           
                            if(!redirectOutput(myargs)){
                                connectToOutputPipe(pfds_next, j, pipeLen);         // connect to pipe output otherwise if required
                            }else{
                                myargs = removeFromVectorAfter(myargs, 1);
                                closeOutputPipe(pfds_next, j, pipeLen);
                            }
                            
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
                        else{
                            closeParentPipe(pfds_prev, j, pipeLen);          // parent goes down this path (original process)
                            if(bgTaskLen < 2){              // vector is length 2 or higher when & was typed
                                status = 0;
                                int wc = wait(&status);
                                if(! WIFEXITED(status))
                                    printf("Program: %d does not terminate normally with exit or _exit \n", wc);
                                else if(WEXITSTATUS(status) != 0)
                                    printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
                            }else{
                                bgTaskLen--; 
                               // printf("Background Program id: %d \n", rc);
                            }
                        }
                    }
                    freeVector(myargs);
                }
            }
            freeVector(bgTaskVec);
            free(pipeVec); // change this
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
int vectorLength(char **vector){
    int cnt=0;
    for(int i=0; vector[i] != 0; i++){
        cnt++;
    }
    return cnt;
    
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
int redirectOutput(char **myargs){
    int wasRedirected = 0;
    if(vectorLength(myargs) == 3 && strcomp(myargs[1], ">")){
        close(1);                               // (STDOUT_FILENO
        open(myargs[2], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        wasRedirected = 1;
    }
    return wasRedirected;
    
}
int redirectInput(char **myargs){
    int wasRedirected = 0;
    if(vectorLength(myargs) == 3 && strcomp(myargs[1], "<")){
        close(0);                               // (STDOUT_FILENO
        open(myargs[2], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        wasRedirected = 1;
    }
    return wasRedirected;
    
}
char **removeFromVectorAfter(char **vector, int index){
    int len, i, j;
    char **newvector;
    
    len = vectorLength(vector);
    if(index > 0 && index < len){
        newvector = (char **)calloc(index+1, sizeof(char *));
        for(i=0; i < index; i++)
            newvector[i] = copystr(vector[i]);
//         for(   ; vector[i+1]; i++)
//             newvector[i] = copystr(vector[i+1]);
        newvector[i] = (char *)0;
    }else{
        newvector = (char **)calloc(1, sizeof(char *));
        newvector[i] = (char *)0;
    }
    free(vector);
    return newvector;
    
}
//                 else{       // then myargs[0] == 0
//                         printf("Error: Pipe syntax error \n");
//                         // handle this error
//                         exit(0);
//                 }


 //                     else {                                              // parent goes down this path (original process)
//                         status = 0;
//                         int wc = wait(&status);
//                         if(! WIFEXITED(status))
//                             printf("Program does not terminate normally with exit or _exit \n");
//                         else if(WEXITSTATUS(status) != 0)
//                             printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
//                     }  
