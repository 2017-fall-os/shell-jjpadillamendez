 
#ifndef vectorLib
#define vectorLib

char **removeFromVectorAfter(char **vector, int index);

char **addToVector(char **vector, char *newItem);

void freeVector(char **tokenVec);

int vectorLength(char **vector);

#endif
