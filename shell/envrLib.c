
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "strlib2.h"
#include "vectorLib.h"
#include "envrLib.h"
#include "mytoc.h"

int setEnvrVar(char **envr, char **myargs){
    int count, argsLen, retVal, len;
    char **argVec;
    
    retVal=1;
    count = countCharAt(myargs[0], '=');
    if(count == 1){                         // CASE: when there is one =
        argsLen = vectorLength(myargs);
        if(argsLen == 1){
            len = strlen2(myargs[0]);
            if(myargs[0][0] == '=' || myargs[0][len-1] == '=')
                write(2, "Error: environment assignment should be var=value\n", 50);
            else{
                argVec = tokenize(myargs[0], '=');
                if( ! addToEnvrVector(envr, argVec[0], argVec[1]))
                    write(2, "Error: environment variable was not found\n", 42);
            }
            
        }
        else{
            write(2, "Error: environment assignment should be var=value\n", 50);
        }
    }
    else if(count > 1){
         write(2, "Error: multiple '=' tokens is not supported\n", 44);
    }
    else{
        retVal=0;                          // CASE: there is no =
    }
    return retVal;
    
}
char **getEnvrVar(char **envr, char *var){
    char **tokenVec, **varVec;
    int i;
    
    varVec = 0;
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            varVec = tokenize(tokenVec[1], ':');  
            freeVector(tokenVec);
            break;
        }
        freeVector(tokenVec);
    }
    if(!varVec){
        varVec = (char **)calloc(1, sizeof(char *));
        varVec[0] = (char *)malloc(1);
        varVec[0] = '\0';
    }
    return varVec;
    
}
char *getEnvrVar2(char **envr, char *var){
    char **tokenVec, *value;
    int i;
    
    value = 0;
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            value = copystr(tokenVec[1]);  
            freeVector(tokenVec);
            break;
        }
        freeVector(tokenVec);
    }
    if(!value){
        value = (char *)malloc(1);
        *value = '\0';
    }
    return value;
    
}
int addToEnvrVector(char **envr, char *var, char*value){
    char **tokenVec, *temp, *temp2;
    int i;
    
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            temp = strconc(var, "=");
            envr[i] = strconc(temp, value);  
            freeVector(tokenVec);
            free(temp);
            return 1;
        }
        freeVector(tokenVec);
    }
    return 0;
    
}
int envrVarLen(char **envr, char *var){
    char **tokenVec;
    int i, len;
    
    len = 0;
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            len = strlen2(tokenVec[1]);  
            freeVector(tokenVec);
            break;
        }
        freeVector(tokenVec);
    }
    return len;
    
}
char **expandEnvrVar(char **envr, char **myargs){
    int i;
    char **newargs, *newarg;
    
    newargs = (char **)calloc(vectorLength(myargs) + 1, sizeof(char *));
    for(i=0; myargs[i]; i++){
        newarg = expandEnvrVar2(envr, myargs[i]);
        newargs[i] = newarg;
    }
    newargs[i] = (char *)0;
  
    return newargs;
}
char *expandEnvrVar2(char **envr, char *myarg){
    int i, j, k, len;
    char *tstr, *newarg, **argVec;
        
    len = 0;
    argVec = tokenize2(myarg, '$');
    for(i=0; argVec[i]; i++){
        if(argVec[i][0] == '\0'){                           // CASE: $ at the beginning
            if(argVec[i+1] && argVec[i+1][0] != '\0'){          // CASE: $HOME
                len += envrVarLen(envr, argVec[i+1]);
                i++;
            }
            else{
                while(argVec[i] && argVec[i][0] == '\0'){      //  CASE: $$$
                    len++;
                    i++;
                }
                if(argVec[i])                                          //CASE: $$$HOME
                    len += strlen2(argVec[i]);
                else
                    break;
            }
        }
        else {                                            // CASE: No $ at the beginning
            len += strlen2(argVec[i]);
        }       
    }
    
    newarg = (char *)malloc(len + 1);
    k = 0;
    for(i=0; argVec[i]; i++){
        if(argVec[i][0] == '\0'){                           // CASE: $ at the beginning
            if(argVec[i+1] && argVec[i+1][0] != '\0'){          // CASE: $HOME
                tstr = getEnvrVar2(envr, argVec[i+1]);
                for(j=0; tstr[j]; j++, k++)
                    newarg[k] = tstr[j];
                free(tstr);
                i++;
            }
            else{
                while(argVec[i] && argVec[i][0] == '\0'){      //  CASE: $$$
                    newarg[k] = '$';
                    k++; i++;
                }
                if(argVec[i]){                                 //CASE: $$$HOME
                    for(j=0; argVec[i][j]; j++, k++)
                        newarg[k] = argVec[i][j];
                }
                else
                    break;
            }
        }
        else {                                            // CASE: No $ at the beginning
            for(j=0; argVec[i][j]; j++, k++)
                newarg[k] = argVec[i][j];
        }       
    }
    newarg[k] = '\0';
    
    return newarg;
    
}
