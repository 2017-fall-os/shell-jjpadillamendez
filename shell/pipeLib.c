 
#include "pipeLib.h" 
#include <stdlib.h>
#include <unistd.h>
 
 
#define FIRST_PROCESS 0
#define LAST_PROCESS pipeLen-1   

#define STDOUT 1
#define STDIN 0

int *connectToInputPipe(int *pfds_prev, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != FIRST_PROCESS){
            close(STDIN);
            dup(pfds_prev[0]);
            close(pfds_prev[0]); close(pfds_prev[1]); 
    
    }  
    
}
int *connectToOutputPipe(int *pfds_next, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != LAST_PROCESS){           // syntax error when pipelen > 1 and myargs[0]
            close(STDOUT);
            dup(pfds_next[1]);
            close(pfds_next[0]); close(pfds_next[1]);
    }  
    
}
void closeOutputPipe(int *pfds_next, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != LAST_PROCESS){
        close(pfds_next[0]); close(pfds_next[1]); 
    }
        
}
void closeInputPipe(int *pfds_prev, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != FIRST_PROCESS){
        close(pfds_prev[0]); close(pfds_prev[1]); 
    }
        
}
void closeParentPipe(int *pfds_prev, int pipe_, int pipeLen){
    if(pipeLen > 1 && pipe_ != FIRST_PROCESS){
        close(pfds_prev[0]); close(pfds_prev[1]); 
        free(pfds_prev);
    }
        
}
