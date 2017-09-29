#ifndef pipeLib
#define pipeLib


int *connectToOutputPipe(int *pfds_prev, int pipe, int pipeLen);

int *connectToInputPipe(int *pfds_next, int pipe, int pipeLen);

void closeInputPipe(int *pfds_prev, int pipe_, int pipeLen);

void closeOutputPipe(int *pfds_prev, int pipe_, int pipeLen);

void closeParentPipe(int *pfds_prev, int pipe, int pipeLen);


#endif
