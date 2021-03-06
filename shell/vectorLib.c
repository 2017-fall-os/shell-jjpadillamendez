

#include <stdlib.h>
#include "vectorLib.h"
#include "strlib2.h"

/** Returns the length of the given vector
 */
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
/** Removes everything in the vector after the given index
 */
char **removeFromVectorAfter(char **vector, int index){
    int len, i, j;
    char **newvector;
    
    len = vectorLength(vector);
    if(index > 0 && index < len){
        newvector = (char **)calloc(index+1, sizeof(char *));
        for(i=0; i < index; i++)
            newvector[i] = copystr(vector[i]);
        newvector[i] = (char *)0;
    }else{
        newvector = (char **)calloc(1, sizeof(char *));
        newvector[i] = (char *)0;
    }
    free(vector);
    return newvector;
    
}
