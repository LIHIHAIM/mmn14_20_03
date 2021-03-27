#include <stdlib.h>
#include <stdio.h>

int *p;

int main(){
    int i, size = 5;
    p = malloc(size * sizeof(int));
    p = realloc(p, ++size * sizeof(int));
    for(i = 0; i < 50; i++){
        p = realloc(p, ++size * sizeof(int));
        p[size-1] = i;
        printf("allocated sucssesfuly\n");
    }
    return 0;
}