
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "strlib2.h"
#include "vectorLib.h"
#include "envrLib.h"
#include "mytoc.h"

/** Change the content of an environment variable ONLY if it 
 *  exits. If the given variable does not exit then the command is ignored
 */
int setEnvrVar(char **envr, char **myargs){
    int count, argsLen, retVal, len;
    char **argVec;
    
    retVal=1;
    count = countCharAt(myargs[0], '=');
    if(count == 1){                         // CASE: when there is one =
        argsLen = vectorLength(myargs);
        if(argsLen == 1){                   // CASE: it must be of the form val=value
            len = strlen2(myargs[0]);
            if(myargs[0][0] == '=' || myargs[0][len-1] == '=')
                fprintf(stderr, "Error: environment assignment should be var=value\n");
            else{
                argVec = tokenize(myargs[0], '=');          // = cannot be at the beginning or end
                if( ! addToEnvrVector(envr, argVec[0], argVec[1]))
                    fprintf(stderr, "Error: environment variable was not found\n");
            }
            
        }
        else{                               // JUST argument value=value; no other tokens 
            fprintf(stderr, "Error: environment assignment should be var=value\n");
        }
    }
    else if(count > 1){
         fprintf(stderr, "Error: multiple '=' tokens is not supported\n");
    }
    else{
        retVal=0;                          // CASE: there is no =
    }
    return retVal;
    
}
/** Returns the content of a environment variable as a vector
 *  in which each entry contains anything before ':' in the value
 */
char **getEnvrVar(char **envr, char *var){
    char **tokenVec, **varVec;
    int i;
    
    varVec = 0;
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            varVec = tokenize(tokenVec[1], ':');     // create environment vector by tokenizing at ':'
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
/** Returns the content of an environment variable as an string
 *  the whole value as a string
 */
char *getEnvrVar2(char **envr, char *var){
    char **tokenVec, *value;
    int i;
    
    value = 0;
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            value = copystr(tokenVec[1]);            // return whole value as string
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
/** Updates the content of an environment variable given its variable
 *  name and the new value
 */
int addToEnvrVector(char **envr, char *var, char*value){
    char **tokenVec, *temp, *temp2;
    int i;
    
    for(i=0; envr[i]; i++){
        tokenVec = tokenize(envr[i], '=');           // tokenize each envr variable to check before '='
        if(strcomp(*tokenVec, var)){
            temp = strconc(var, "=");
            envr[i] = strconc(temp, value);          // var=value
            freeVector(tokenVec);
            free(temp);
            return 1;
        }
        freeVector(tokenVec);
    }
    return 0;
    
}
/** Returns the length of the value of an environment variable
 *  anything after var=
 */
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
/** Expands the given arguments if its contains non-embedded ($HOME) OR 
 *  embedded(${HOME}) environment variables
 */
char *expandEnvrVar(char **envr, char *myarg){
    int i, j, k, len, closed;
    char *tstr, *tstr2, *newarg, **argVec, varlen;
        
    len = 0;                                                // FIRST COUNT THE LEGTH REQUIRED OF THE NEW EXPANDED STRING
    argVec = tokenize2(myarg, '$');
    for(i=0; argVec[i]; i++){
        if(argVec[i][0] == '\0'){                           // CASE: $ at the beginning
            if(argVec[i+1] && argVec[i+1][0] != '\0'){          // CASE: $HOME
                if(argVec[i+1][0] == '{'){                          // CASE: when it is embedded
                    closed = 0;
                    varlen = 0;
                    for(j=0; argVec[i+1][j+1]; j++){
                        if(argVec[i+1][j+1] == '}'){
                            len += strlen2(&argVec[i+1][j+2]);      // count rest: ${HOME}rest
                            if(j > 0) closed = 1;
                            break;
                        }
                        varlen++;
                    }
                    if(closed){                                     // closed means: ${HOME}
                        tstr = (char *)malloc(varlen + 1);
                        for(j=0; j < varlen; j++){
                            tstr[j] = argVec[i+1][j+1];             // copy value of environment variable
                        }
                        tstr[j] = '\0';
                        len += envrVarLen(envr, tstr);              // then count the length of the environment variable
                        free(tstr);
                    }
                }
                else{
                    len += envrVarLen(envr, argVec[i+1]);           // CASE: NOT embedded simple $ANY
                }
                i++;                // jump to after $ANY OR ${ANY}
            }
            else{
                while(argVec[i] && argVec[i][0] == '\0'){          //  CASE: $$$
                    len++;
                    i++;
                }
                if(argVec[i])                                      //CASE: $$$HOME
                    len += strlen2(argVec[i]);
                else
                    break;
            }
        }
        else {                                            // CASE: No $ at the beginning
            len += strlen2(argVec[i]);
        }       
    }
    
    newarg = (char *)malloc(len + 1);                           // SECOND: COPY THE CONTENT TO THE NEW EXPANDED VARIABLE
    k = 0;
    for(i=0; argVec[i]; i++){
        if(argVec[i][0] == '\0'){                           // CASE: $ at the beginning
            if(argVec[i+1] && argVec[i+1][0] != '\0'){          // CASE: $HOME
                if(argVec[i+1][0] == '{'){                          // CASE: when it is embedded
                    closed = 0;
                    varlen = 0;
                    for(j=0; argVec[i+1][j+1]; j++){                // count again the length of the current envr variable
                        if(argVec[i+1][j+1] == '}'){
                            if(j > 0) closed = 1;
                            break;
                        }
                        varlen++;
                    }
                    if(closed){                                    // closed means: ${HOME}
                        tstr2 = (char *)malloc(varlen + 1);
                        for(j=0; j < varlen; j++){
                            tstr2[j] = argVec[i+1][j+1];           // Get name of possible envr variable
                        }
                        tstr2[j] = '\0';
                        tstr = getEnvrVar2(envr, tstr2);           // Get envr variable VALUE
                        for(j=0; tstr[j]; j++, k++)
                            newarg[k] = tstr[j];                   // copy the content of the value to the expanded string
                        free(tstr);
                        free(tstr2);
                        
                        for(j=0; argVec[i+1][j+2+varlen]; j++, k++)         // Copy the rest   ${HOME}rest
                            newarg[k] = argVec[i+1][j+2+varlen];
                    }
                }
                else{                                             // CASE: NOT embedded envr variable; simple $HOME
                    tstr = getEnvrVar2(envr, argVec[i+1]);
                    for(j=0; tstr[j]; j++, k++)
                        newarg[k] = tstr[j];
                    free(tstr);
                }
                i++;                    // skip variable name
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
