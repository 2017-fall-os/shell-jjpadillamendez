 
#include "pipeLib.h" 
#include <stdlib.h>
#include <unistd.h>
 
 
#define FIRST_PROCESS 0
#define LAST_PROCESS pipeLen-1   

#define STDOUT 1
#define STDIN 0

int *pfds_prev, *pfds_next;     

/** Init the pipe for the case when only needed for input OR output AND for both cases
 */
void initPipe(int process, int pipeLen){
    if(pipeLen > 1){
        if(process != FIRST_PROCESS){
        pfds_prev = pfds_next;                             // pipe ready to take input
        }
        if(process != LAST_PROCESS){                         // pipe ready to send output
            pfds_next = (int *)calloc(2, sizeof(int));
            pipe(pfds_next);
        }
    }
    
}
/** Connect the process to the input pipe; the only case where a process is the first 
 *  process in the pipe line
 */
int *connectToInputPipe(int process, int pipeLen){
    if(pipeLen > 1 && process != FIRST_PROCESS){
            close(STDIN);
            dup(pfds_prev[0]);
            close(pfds_prev[0]); close(pfds_prev[1]); 
    
    }  
    
}
/** Connect the process to the ouput pipe; the only case where a process is the last
 *  process in the pipe line
 */
int *connectToOutputPipe(int process, int pipeLen){
    if(pipeLen > 1 && process != LAST_PROCESS){           // syntax error when pipelen > 1 and myargs[0]
            close(STDOUT);
            dup(pfds_next[1]);
            close(pfds_next[0]); close(pfds_next[1]);
    }  
    
}
/** Close the output pipe for everyprocess except for the lastone in the pipeline
 */
void closeOutputPipe(int process, int pipeLen){
    if(pipeLen > 1 && process != LAST_PROCESS){
        close(pfds_next[0]); close(pfds_next[1]); 
    }
        
}
/** Close the input pipe for everyprocess except for the first in the pipeline
 */
void closeInputPipe(int process, int pipeLen){
    if(pipeLen > 1 && process != FIRST_PROCESS){
        close(pfds_prev[0]); close(pfds_prev[1]); 
    }
        
}
/** Close the parent pipe only for the previous already unused pipe
 */
void closeParentPipe(int process, int pipeLen){
    if(pipeLen > 1 && process != FIRST_PROCESS){
        close(pfds_prev[0]); close(pfds_prev[1]); 
        free(pfds_prev);
    }
        
}
