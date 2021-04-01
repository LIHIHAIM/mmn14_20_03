#include <stdio.h>
#include "bool.h"

/* from symbolTable.c */
void pushEntryToFile(FILE *);
void pushExternToFile(FILE *);
boolean entryExist();
boolean externalExist();
void pushInstsToFile(FILE *);   /* from instructionHandeling.c */
void pushDataToFile(FILE *);    /* from dataHandeling.c */