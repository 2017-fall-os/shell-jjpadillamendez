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
#define CASE1 1
#define CASE2 0

int main(int argc, char **argv, char**envp){
	  
    char **myargs, **path, **pipeVec;
    char **commandVec, *program, **bgTaskVec, **redirectVec;
    int rc, status, pipeLen, bgTaskLen;
    
    while(1){
        commandVec = waitForUserCommand();                // Get input command
        for(int i=0; commandVec[i]; i++){
            if(!isEmpty(commandVec[i])){                  // empty commands should do nothing
                bgTaskVec = tokenizeAndCheckSyntax(commandVec[i], '&', CASE1);
                if(bgTaskVec){
                    bgTaskLen = vectorLength(bgTaskVec);
                    for(int k=0; bgTaskVec[k]; k++){
                        pipeVec = tokenizeAndCheckSyntax(bgTaskVec[k], '|', CASE2);
                        if(pipeVec){
                            pipeLen = vectorLength(pipeVec);
                            for(int j=0; pipeVec[j]; j++){
                                myargs = tokenize(pipeVec[j], ' ');                    // Generate command arguments vector
                                if(myargs[0]){                              // como remover esto
                                    initPipe(j, pipeLen);                   // Init pipe just if needed
                                    rc = fork2();
                                    if(rc == 0) {                                  // CHILD PROCESS 
                                        // Redirect input to a text file if required
                                        if(!redirectInput(myargs)){
                                            connectToInputPipe(j, pipeLen);         // connect to pipe input otherwise if required
                                        }else{
                                            myargs = removeFromVectorAfter(myargs, 1);
                                            closeInputPipe(j, pipeLen);
                                        }
                                        // Redirect output to a text file if required           
                                        if(!redirectOutput(myargs)){
                                            connectToOutputPipe(j, pipeLen);         // connect to pipe output otherwise if required
                                        }else{
                                            myargs = removeFromVectorAfter(myargs, 1);
                                            closeOutputPipe(j, pipeLen);
                                        }
                                        executeChild(myargs, envp);                                    // based on my point of view, a command not found should return 0
                                        
                                    } 
                                    else{
                                        closeParentPipe(j, pipeLen);          // parent goes down this path (original process)
                                        if(bgTaskLen < 2){              // vector is length 2 or higher when & was typed
                                            status = 0;
                                            int wc = waitpid(rc, &status, 0);
                                            if(! WIFEXITED(status))
                                                printf("Program: %d does not terminate normally with exit or _exit \n", wc);
                                            else if(WEXITSTATUS(status) != 0)
                                                printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
                                        }else{
                                            bgTaskLen--;
                                        }
                                    }
                                }
                                freeVector(myargs);
                            }
                        }else{
                            printf("Syntax Error: near an unexpected token '|' \n");
                            break;
                        }
                    }
                    freeVector(bgTaskVec);
                    free(pipeVec); // change this
                }else{
                    printf("Syntax Error: near an unexpected token '&' \n");
                    // break if ; is introduced
                }
            }
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
void executeChild(char **myargs, char **envp){
    char *program, **path;
    
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
    exit(0);

}
int fork2(){
    int rc;
    rc = fork();
    if (rc < 0) {                                  
        fprintf(stderr, "Error: fork() failed\n");
        exit(1);
    }
    return rc;
    
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
        open(myargs[2], O_RDONLY);
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
char **tokenizeAndCheckSyntax(char *str, char delimer, int case_syntax){
    int delimLen, vectorLen, i, k;
    char *tempstr, *tempstr2, **tokenVec;
    char error = 0, *msg;
    
    delimLen = countCharAt(str, delimer);
    
    if(delimLen > 0){
        tempstr2 = strconc(" ", str);
        tempstr =strconc(tempstr2, " ");           // Quick way to avoid removing delimer at the start and end of str
        tokenVec = tokenize(tempstr, delimer);
        
        if(delimLen == vectorLength(tokenVec)-1){
            k = (case_syntax) ? 0 : 1;
            for(i=0; i < delimLen + k; i++){        
                if(isEmpty(tokenVec[i])){
                    error = 1;
                    break;
                }
            }
        }
        else{
            error = 1;              // repeated delimers are not allowed
        }
        
        if(error){
            freeVector(tokenVec);
            tokenVec = 0;
        }
        free(tempstr);
        free(tempstr2);
    }
    else{
        tokenVec = tokenize(str, delimer);
    }
    return tokenVec;
    
}


 //                     else {                                              // parent goes down this path (original process)
//                         status = 0;
//                         int wc = wait(&status);
//                         if(! WIFEXITED(status))
//                             printf("Program does not terminate normally with exit or _exit \n");
//                         else if(WEXITSTATUS(status) != 0)
//                             printf("Program terminated with exit code %d \n", WEXITSTATUS(status));
//                     }  
