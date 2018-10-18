#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) 
{
    printf(" :: ");
    char* gotLine = (char*) calloc(256, sizeof(char));
    getTermLine(gotLine);
    puts(gotLine);
    return (EXIT_SUCCESS);
}

