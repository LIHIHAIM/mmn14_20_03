/* [mmn 14 : bool.h]:
in this file: Defenition of the two boolean types: true and false + an ERROR constant.

author: Uri K.H,   Lihi Haim       Date: 21.3.2021 
ID: 215105321,     313544165       Tutor: Danny Calfon */

#ifndef BOOL_H
#define BOOL_H
typedef enum {FALSE /*= 0*/, TRUE /*= 1*/, ERROR = -1 } boolean;
/* The ERROR constant represent a situation when an unexpected error acurred */
/*#define REALLOCATE(pointer, prev_size, new_size, type)
    {
        int i;
        type *temp = malloc(new_size * sizeof(type));
        for(i = 0; i < prev_size; i++)
            temp[i] = pointer[i];
        free(pointer);

    }*/
#endif