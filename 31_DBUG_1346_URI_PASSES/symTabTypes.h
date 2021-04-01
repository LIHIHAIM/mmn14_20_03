#ifndef SYMTABTYPES_H
#define SYMTABTYPES_H

#define MAX_LABEL_LEN 31
enum attribs{ EXT = 1, ENT, DAT, COD };

typedef struct {
    char symbol[MAX_LABEL_LEN + 1];
    int address;
    enum attribs attribute1;
    enum attribs attribute2;
} symbolTable;

#endif