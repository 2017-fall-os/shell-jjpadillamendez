#ifndef pipeLib
#define pipeLib


int *connectChildToPipe(int *pfds_prev, int *pfds_next, int pipe, int pipeLen);

void closeParentPipe(int *pfds_prev, int pipe, int pipeLen);


#endif
