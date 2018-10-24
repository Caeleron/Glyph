#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal.h"

#define EXIT "exit"

int main(int argc, char** argv) 
{    
    char* gotLine;
    
    do
    {
        gotLine = getTermLine();
    } while(strcmp(gotLine, EXIT));
    puts(gotLine);
    return (EXIT_SUCCESS);
}

