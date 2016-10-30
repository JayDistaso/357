#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ftw.h>
#include <dirent.h>
#include <fcntl.h> 
#include <errno.h>
#include <error.h>
#include <ctype.h>
#include <assert.h>

#define UNIX_DICT "/usr/share/dict/words"
#define MAX_LINE_LENGTH 1023
#define MAX_WORD_SIZE 100

void GetFlags(int argc, char **argv);
void TrimString();
char *StripChars(char *string);
int CompareLine(char *line);
int Look();
char *GetLine(FILE *file);
