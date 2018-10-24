#ifndef TERMINAL_H
#define TERMINAL_H

char getTermChar();
void insertch(char ch, int idx, char* dest);
void refreshLine(char* str, int len);
char* getTermLine();

#endif /* TERMINAL_H */

