#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"

// Giving the interpreter short term memory. 
char** inputHistory = 0; // Array of strings
int inputHistoryIdx = 0; // Indexer; currently active string in history
int inputHistoryCnt = 0; // Number of strings in memory

/*
 * Custom implementation of getch() for this distribution.
 */
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

void insertch(char ch, int idx, char* currentStr)
{
    char tmp;
    while(ch != '\0')
    {
        tmp = currentStr[idx];
        currentStr[idx++] = ch;
        ch = tmp;
    }
}

/*
 * Clears the current line on the terminal and reprints the contents of the current active string.
 * 
 * Used when a character is inserted or the active string was changed.
 */
void refreshLine(char* str, int len)
{ 
    printf("\0337"); // Save the cursor position so we can mess with it and not mess up getTermLine().
    printf("\033[2K"); // Clear the current line.
    printf("\033[%dD", len+4); // Move the cursor to the leftmost position on the line.
    printf(" :: %s", str); // Print the string with the prefix.
    printf("\0338"); // Restore the cursor position.
}

/*
 * Fills provided string with input from terminal. 
 */
char* getTermLine()
{   
    char* currentStr = calloc(256, sizeof(char)); 
    
    int writtenLength = 0; // Remembers the amount of characters in the string so strlen() doesn't need to get run constantly.
    
    printf(" :: "); // Print the initial prefix.
    
    // If inputHistory is not initialized, initialize it.
    if(inputHistory == 0) 
    {
        inputHistory = (char**) calloc(64, sizeof(char*));
    }
    
    // Store the string's pointer in history and increase the counter.
    inputHistory[inputHistoryIdx] = currentStr;
    inputHistoryCnt++;
    
    // Indexer for supplied string initialized at the leftmost character.
    int currentStrIdx = 0;
    
    // Get character input until a CR
    char ch;
    do
    {
        ch = getTermChar();
        char tmp = '\0'; // Temporary character variable for use in character insertions
        switch(ch)
        {
            case '\n': // CR; conclude character reading
                // Make sure the cursor is at the end of string.
                while(currentStr[currentStrIdx] != '\0') 
                { 
                    currentStrIdx++;
                    printf("\033[1C"); // Move cursor right one character.
                }
                putchar('\n'); // Print the CR back to the terminal.
                
                // If the user didn't make a new entry and used one from history, decrement the counter and dealloc the new string.
                if(inputHistoryIdx != inputHistoryCnt - 1)
                {
                    inputHistoryCnt--;
                    free(inputHistory[inputHistoryCnt]);
                }
                
                inputHistoryIdx = inputHistoryCnt; // Set the history indexer to after the last string.
                break;
            case '\b': // Backspace  
                if(currentStrIdx > 0) // Make sure we are backspacing outside of the string.
                {
                    // Shift characters to remove the selected character in the string.
                    for (int i = writtenLength-- - 1; i >= currentStrIdx - 1 ; i--) 
                        {
                            ch = currentStr[i];
                            currentStr[i] = tmp;
                            tmp = ch;
                        }
                    refreshLine(currentStr, currentStrIdx--);
                    printf("\033[1D"); // Move cursor left one character.
                }
                break;
            case '\033': // Escape sequence
                getTermChar(); // Discard this character
                ch = getTermChar();
                switch (ch)
                {
                    case 'A': // Up arrow; get previous entry in history
                        if(inputHistoryIdx > 0) // Make sure we aren't at the first entry
                        {
                            currentStr = inputHistory[--inputHistoryIdx]; // Set the active string to next string in history. 
                            printf("\033[%dD", currentStrIdx); // Make sure the cursor is after the prefix.
                            currentStrIdx = 0; // Reset the string indexer to the start of the string
                            writtenLength = strlen(currentStr) - 1; // Get the new length of the string.
                            refreshLine(currentStr, 0); // Refresh so the user sees the string in history.
                        }
                        
                        break;
                    case 'B': // Down arrow; get next entry in history
                        if(inputHistoryIdx < inputHistoryCnt - 1) // Make sure we aren't at the last entry.
                        {
                            currentStr = inputHistory[++inputHistoryIdx]; // Set the active string to previous string in history. 
                            printf("\033[%dD", currentStrIdx); // Make sure the cursor is after the prefix.
                            currentStrIdx = 0; // Reset the string indexer to the start of the string
                            writtenLength = strlen(currentStr) - 1; // Get the new length of the string.
                            refreshLine(currentStr, 0); // Refresh so the user sees the string in history.
                        }
                        break;
                    case 'C': // Right arrow; move cursor right. 
                        if(currentStrIdx < writtenLength)
                        {
                            currentStrIdx++;
                            printf("\033[1C");
                        }
                        break;
                    case 'D': // Left arrow; move cursor left.
                        if(currentStrIdx > 0)
                        {
                            currentStrIdx--;
                            printf("\033[1D");
                        }
                        break;
                }
                break;
            default: // Standard character or random special character I don't know about
                insertch(ch, currentStrIdx++, currentStr);
                refreshLine(currentStr, currentStrIdx);
                printf("\033[1C");
                writtenLength++;
        };
    } while(ch != '\n');
    return currentStr;
}