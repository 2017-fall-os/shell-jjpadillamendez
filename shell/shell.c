/********************************************************************/
/* Shell: Lab Assignment 2, Part 2         Date: 10/1/2017          */
/* Author: Jesus Padilla                   Class: Operating System  */
/* Prof: Dr. Freudenthal                   TA: Adrian Veliz         */
/* This program implements a shell which executes simple commands   */
/* and other advances features                                      */
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
#include "vectorLib.h"
#include "envrLib.h"

#define BUFFERLIMIT 102                             // Include space for '/n' and '/0'
#define STDOUT 1                                    // standard output
#define STDIN  0                                    // standard input

#define CASE1 1
#define CASE2 0

int main(int argc, char **argv, char**envp){
	  
    char **commandVec, **bgTaskVec, **pipeVec, **myargs, **redirectVec;
    char *expadArg, *program;
    int rc, status, pipeLen, bgTaskLen;
        
    while(1){
        commandVec = waitForUserCommand(envp);                                  // Step 1: Read command from the user
        for(int i=0; commandVec[i]; i++){
            if(bgTaskVec = tokenizeAndCheckSyntax(commandVec[i], '&', CASE1)){   // Step 2: check which jobs are to run in the background
                bgTaskLen = vectorLength(bgTaskVec);        
                for(int k=0; bgTaskVec[k]; k++){
                    if(pipeVec = tokenizeAndCheckSyntax(bgTaskVec[k], '|', CASE2)){     // Step 3: check if pipes are needed
                        pipeLen = vectorLength(pipeVec);
                        for(int j=0; pipeVec[j]; j++){
                            expadArg = expandEnvrVar(envp, pipeVec[j]);                  // Step 3.1: Expand non-embedded and embedded variables that starts with $
                            myargs = tokenize(expadArg, ' ');                         // Step 4: Separate each process into its arguments
                            if(myargs[0] && !isAnEmbeddedCmd(envp, myargs)){                             
                                initPipe(j, pipeLen);                                   // Step 4.1: Init pipe just if needed
                                rc = fork2();
                                if(rc == 0) {                                           // Child process is here
                                                                                        // Step 5: Redirect I/0 in the child process
                                    if(!redirectInput(myargs)){
                                        connectToInputPipe(j, pipeLen);                 // if input redirection to file is required; pipes are closed
                                    }else{
                                        myargs = removeFromVectorAfter(myargs, 1);
                                        closeInputPipe(j, pipeLen);
                                    }
                                    if(!redirectOutput(myargs)){                        // if output redirection to file is required; pipes are closed
                                        connectToOutputPipe(j, pipeLen);         
                                    }else{
                                        myargs = removeFromVectorAfter(myargs, 1);
                                        closeOutputPipe(j, pipeLen);
                                    }
                                    executeChild(myargs, envp);                         // Step 5: Everything is ready, run child process

                                } 
                                else{                                                   // Parent process (shell) goes down here
                                    closeParentPipe(j, pipeLen);                        // Close its pipe if it was required by child
                                    if(bgTaskLen < 2){                                  
                                        status = 0;
                                        int wc = waitpid(rc, &status, 0);               // waiting for child if it is NOT running in the background
                                        if(! WIFEXITED(status))
                                            fprintf(stderr, "Program: %d does not terminate normally with exit or _exit \n", wc);
                                        else if(WEXITSTATUS(status) != 0){
                                            fprintf(stderr, "Program terminated with exit code %d \n", WEXITSTATUS(status));
                                            break;
                                        }
                                    }else{
                                        bgTaskLen--;                                    // bgTaskLen is the sum of the background jobs; decrease after completion
                                    }
                                }
                            }
                            freeVector(myargs);
                            free(expadArg);
                        }
                    }else{
                        fprintf(stderr, "Syntax Error: near an unexpected token '|' \n");
                        break;
                    }
                }
                freeVector(bgTaskVec);
                freeVector(pipeVec); 
            }else{
                fprintf(stderr, "Syntax Error: near an unexpected token '&' \n");
            }
        }
        freeVector(commandVec);

    }
    return 0;
    
}
/** Returns a vector in which each entry contains a command
 *  from the user
 */
char **waitForUserCommand(char **envp){    
    int len;
    char *str = (char *)malloc(BUFFERLIMIT);
    char **commandVec;
    char *PS1 = getEnvrVar2(envp, "PS1");
    len = strlen2(PS1);
    
    write(1, PS1, len);
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
/**
 * executes a child process given the arguments which contains the new program
 * and its parameters; additionally, a environment variables are required
 */
void executeChild(char **myargs, char **envp){
    char *program, **path;
    
    execve(myargs[0], myargs, envp);                // Run command as it is ..     
                                            
    path = getEnvrVar(envp, "PATH");                // .. If previous execve returns, than check path environment
    program = strconc("/", myargs[0]);
    while(*path){
        free(myargs[0]);
        myargs[0] = strconc(*path, program);        // Run path/program    
        execve(myargs[0], myargs, envp);
        path++;
    }
    fprintf(stderr, "Error: Command was not found\n");          // execve only returns on upon an error
    exit(2);
    
}
/** Change directory (cd) command is implemented in this shell
 *  it is NOT implemented in a child process
 */
int isChangeDir(char **envp, char **myargs){
    int retval;
    char *args;
    if(strcomp(myargs[0], "cd")){
        if(myargs[1] == '\0')                   // if no arguments are provided, go to HOME by default
             myargs[1] = getEnvrVar2(envp, "HOME");
        if(chdir(myargs[1]) < 0)                // Other arguments are ignored
            fprintf(stderr, "Error: specified absolute or relative path does not exit.\n");
        retval = 1;
    }
    else{
        retval = 0;
    }
    return retval;
    
}
/** The same fork; just in this method is included the possible 
 *  error message
 */
int fork2(){
    int rc;
    rc = fork();
    if (rc < 0) {                                  
        fprintf(stderr, "Error: fork() failed\n");
        exit(1);
    }
    return rc;
    
}
/** Check if the given command is an embedded command of the shell
 *  in which case; it is not needed to create a child process
 */
int isAnEmbeddedCmd(char **envp, char **myargs){
    if(isChangeDir(envp, myargs) || setEnvrVar(envp, myargs))
        return 1;
    else
        return 0;
}
/** Redirect the standard output to a given file '>' if requested
 *  otherwise it does not do anything and returns 0
 */
int redirectOutput(char **myargs){
    int wasRedirected = 0;
    if(vectorLength(myargs) == 3 && strcomp(myargs[1], ">")){
        close(STDOUT);                               // (STDOUT_FILENO
        open(myargs[2], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
        wasRedirected = 1;
    }
    return wasRedirected;
    
}
/** Redirect the standard output to a given file '>' if requested
 *  otherwise it does not do anything
 */
int redirectInput(char **myargs){
    int wasRedirected = 0;
    if(vectorLength(myargs) == 3 && strcomp(myargs[1], "<")){
        close(STDIN);                               
        open(myargs[2], O_RDONLY);
        wasRedirected = 1;
    }
    return wasRedirected;
    
}
/** Tokenize and checks that the given command is in the correct syntax
 *  case 1: when we have something like cmd & cmd & in which only to the left is required a cmd
 *  case 0: when we have cmd | cmd | cmd in which we need to both sides always a cmd
 */
char **tokenizeAndCheckSyntax(char *str, char delimer, int case_syntax){
    int delimLen, vectorLen, i, k;
    char *tempstr, *tempstr2, **tokenVec;
    char error = 0, *msg;
    
    delimLen = countCharAt(str, delimer);           // count number of delimers in the string
    
    if(delimLen > 0){
        tempstr2 = strconc(" ", str);
        tempstr =strconc(tempstr2, " ");           // Quick way to avoid removing delimer at the start and end of str
        tokenVec = tokenize(tempstr, delimer);
        
        if(delimLen == vectorLength(tokenVec)-1){
            k = (case_syntax) ? 0 : 1;             // Selecting which case we need
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
