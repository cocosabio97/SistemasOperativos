#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>

typedef enum{
  NONE,
  ERROR,
  CREATE,
  ALTERNATIVE_CREATE,
  EXTRACT,
  ALTERNATIVE_EXTRACT
} flags;

typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);
int createAlternative(int nFiles, char *fileNames[], char tarName[]);
int extractAlternative(char tarName[]);


#endif /* _MYTAR_H */
