 
#include "pipeLib.h" 
#include <stdlib.h>
#include <unistd.h>
 
 
#define FIRST_IN_LINE 0
#define LAST_IN_LINE pipeLen-1   

int *connectChildToPipe(int *pfds_prev, int *pfds_next, int pipe_, int pipeLen){
    if(pipeLen > 1){
        if(pipe_ != FIRST_IN_LINE){
            close(0);
            dup(pfds_prev[0]);
            close(pfds_prev[0]); close(pfds_prev[1]); 
        }
        if(pipe_ != LAST_IN_LINE){                                 // syntax error when pipelen > 1 and myargs[0]
            close(1);
            dup(pfds_next[1]);
            close(pfds_next[0]); close(pfds_next[1]);
        }
    }  
    
}

void closeParentPipe(int *pfds_prev, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != FIRST_IN_LINE){
        close(pfds_prev[0]); close(pfds_prev[1]); 
        free(pfds_prev);
    }
        
}
