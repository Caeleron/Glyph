#ifndef TERMINAL_H
#define TERMINAL_H

int writtenLength = 0;

void initHistory();
char getTermChar();
void insertch(char ch, int idx, char* dest);
void refreshLine(char* str, int len);
void getTermLine(char* dest);

#endif /* TERMINAL_H */

