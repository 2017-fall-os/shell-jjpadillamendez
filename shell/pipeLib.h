#ifndef pipeLib
#define pipeLib

void initPipe(int pipe_, int pipeLen);

int *connectToOutputPipe(int pipe, int pipeLen);

int *connectToInputPipe(int pipe, int pipeLen);

void closeInputPipe(int pipe_, int pipeLen);

void closeOutputPipe(int pipe_, int pipeLen);

void closeParentPipe(int pipe, int pipeLen);


#endif
