#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INIT_CAPACITY 10
#define DOUBLE 2

typedef struct Rule {
   char *name;
   int capacityDepends;
   int capacityActions;
   int numDepends;
   int numActions;
   char **depends;
   char **actions;
   int ruleIndex;
} Rule;

struct Rule *MakeRule(char *tok, int numRules);
int FindRule(Rule **rulebook, char *name, int numRules);
int ExecuteDepends(Rule **rules, int numRules, int indexRule, 
 int indexDepends, int updated, int rootIndex);
int FileExist(char *filename);
void ExecuteArgs(int argc, int ruleIndex, Rule **rules, char *argv[], int numRules, Rule *currRule, int ndx, int flag, int temp);
void ExecuteEnd(int validDepends, int executeMe, int indexRule, int rootIndex, int ndx, Rule **rules, int numRules, int updated, int temp, int indexDepends);
int ParseFile(FILE *filePointer, Rule **rules);
void printRule(Rule *r);



