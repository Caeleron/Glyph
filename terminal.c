#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

char getTermChar()
{
    struct termios curAttr, getchAttr;
    
    tcgetattr( STDIN_FILENO, &curAttr );
    
    getchAttr = curAttr;
    getchAttr.c_lflag &= ~( ICANON | ECHO );
    
    tcsetattr( STDIN_FILENO, TCSANOW, &getchAttr );
    
    char ch = getchar();
    
    tcsetattr( STDIN_FILENO, TCSANOW, &curAttr );
    
    return ch;
}

void insertch(char ch, int idx, char* dest)
{
    char tmp;
    while(ch != '\0')
    {
        tmp = dest[idx];
        dest[idx++] = ch;
        ch = tmp;
    }
}

void refreshLine(char* str, int len)
{ 
    printf("\0337");
    printf("\033[2K");
    printf("\033[%dD", len+4);
    printf(" :: %s", str);
    printf("\0338");
}

void getTermLine(char* dest)
{
    // Custom buffer and indexer
    int destIdx = 0;
    
    // Get character input until a CR
    char ch;
    do
    {
        ch = getTermChar();
        switch(ch)
        {
            case '\n':
                while(dest[destIdx] != '\0') 
                { 
                    destIdx++;
                    printf("\033[C");
                }
                insertch('\n', destIdx++, dest);
                putchar('\n');
                break;
            case '\b': // Backspace
                break;
            case '\033':
                getTermChar();
                ch = getTermChar();
                switch (ch)
                {
                    case 'C':
                        destIdx++;
                        printf("\033[C");
                        break;
                    case 'D':
                        destIdx--;
                        printf("\033[D");
                        break;
                }
                break;
            default: // Standard character or random special character I don't know about
                insertch(ch, destIdx++, dest);
                refreshLine(dest, destIdx);
                printf("\033[1C");
        };
    } while(ch != '\n');
}