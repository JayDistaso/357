#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
//#include "SmartAlloc.h"
#include "smake.h"
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

//SMAKE
struct Rule *makeRule(char*, int);
void printRule(Rule*);
int findRule (Rule**, char*, int);
int executeDepends(Rule**, int, int, int, int, int);
int file_exist (char*); 

int main(int argc, char *argv[]) {
   printf("Making variables\n");
   FILE *fp;
   const char delim = '\t';
   char * tok;
   char * line = NULL;
   int ruleCapacity = 10;
   Rule **rules = calloc(10, sizeof(Rule *)); //Grow as needed? 
   size_t len = 0;
   ssize_t read;
   int i = 0;
   int numRules = 0;
   char *str;
   Rule *currRule;
   int location;
   int index;
   int flag = 0;
   int validDepends = 0;
   
   printf("Done amkig varibales\n\n");
   //int makingDepends = 0;
   //using fopen lets you use ftell() for the position in the file, as well as fseek() to set position including relative to start end or position!
   printf("opening file\n");
   if ((fp = fopen("Smakefile", "r")) == NULL) {
      printf("File failed to open\n");
      exit(-1);
   }
   //inital parseing
   //line will be address of buffer, len will be allocated apace
   //do not forget to free buffer when its all done with
   printf("Starting while\n\n");
   while ((read = getline(&line, &len, fp)) != -1) { //reads line by line until end of file
      str = strdup(line);//mallocs new string, needs to be freed with free if not used, which it is always used I believe (except on null chars?)
         printf("the string we are workign with is %s\n\n", str);
         if (str[0] == '\t') { //The first thing in this line is tab, its an action
            str[strlen(str) - 1] = '\0'; //strip newline into null
            currRule->actions[currRule->numActions] = strdup(str + 1); //action is stored in one slot of action////////////////////////CRASH HERE
            if (currRule->numActions > currRule->capacityActions - 2) {
               currRule->capacityActions *= 2;
               currRule->actions = realloc(currRule->actions, currRule->capacityActions * sizeof(currRule->actions));
            }
            currRule->numActions++;
         }
         else if (str[0] != '\n' && str[0] != ' ')  {//non-blank line is a rule
            tok = strtok(str, " \n");
            if (tok) {//Go through token by token, this check is prob redudant
               printf("RULE ADDED\n");
               rules[numRules] = makeRule(tok, numRules);
               printf("pointer to book was good\n");
               currRule = rules[numRules++];
               printf("handover to curr was good\n");
               if (numRules > ruleCapacity - 2) {
                  printf("start reziing rule book\n");
                  ruleCapacity *= 2;
                  rules = realloc(rules, ruleCapacity * sizeof(*rules));
                  printf("end resizing rule book\n"); 
               }
               printf("tokenizing\n"); 
               tok = strtok(NULL, " \n");
               printf("Finished tokenizing took = %s_\n", tok);
               if (strcmp(tok, ":")) {//eat / check for colon
                  printf("Error missing :\n");
                  exit(-1);
               }
               tok = strtok(NULL, " \n");
               while (tok) {
                  currRule->depends[currRule->numDepends] = strdup(tok);
                  if (currRule->numDepends > currRule->capacityDepends - 2) {
                     currRule->capacityDepends *= 2;
                     currRule->depends = realloc(currRule->depends, currRule->capacityDepends * sizeof(currRule->depends));
                  }
                  currRule->numDepends++;
                  tok = strtok(NULL, " \n"); 
               }
            }                       
         }
         else {
            free(str); //should only be here for blank lines
            printf("In blank line if\n");
         }       
   } //After this while we should have all the rule names as well as their respective actions and depends stored in *rules
   
   rewind(fp); //go back to start of file
   
  printf("----------------------DONE PARSING-------------------\n\n");
   for (i = 0; i < numRules; i++) {
      printf("Info for rule %d\n", i);
      printRule(rules[i]);
   }
   printf("----------------------DONE VERIFING---------------------\n\n");
   //end parsing :D
   //Set root 
   if (argc == 2) {
      index = findRule(rules, argv[1], numRules);
      if (index == -1) {
         printf("The command line rule noes not exsist\n");
         exit -1;
      }
      currRule = rules[index];
   }
   else if (argc == 1) {
      currRule = rules[0]; //The first rule
   }
   else {
      printf("too many command line arguments\n");
      exit (-1);
   }
   //old is 0
   printf("past arg check\n");
   for (i = 0; i < currRule->numDepends; i++) {//Check each depends
   printf("running depend[%d]\n", i);
      if (executeDepends(rules, numRules, currRule->index, i, 0, currRule->index)) { //returns 1 for updated
         flag = 1;
      } 
   }
   if (flag) {
      for (i = 0; i < rules[currRule->index]->numActions; i++) {
         printf("Executing command: %s\n", rules[currRule->index]->actions[i]);
         system(rules[currRule->index]->actions[i]);      
      }
   }
   
   //close stream, free buffer
   fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
    
}

struct Rule *makeRule(char *tok, int numRules)
{
   printf("Building a rule struct with name %s\n", tok);
   struct Rule *r = calloc(1, sizeof(Rule));
   r->name = strdup(tok);
   r->index = numRules;
   r->actions = calloc(10, sizeof(char *));
   r->depends = calloc(10, sizeof(char *));
   r->capacityDepends = 10;
   r->capacityActions = 10;
   r->numDepends = 0;
   r->numActions = 0;
   printf("build was guuci\n");
   return r;
}

void printRule(Rule *r) {
   int i;
   printf("Index is %d\n\n", r->index);
   printf("Rule name is %s\n", r->name);
   printf("Rule capacity is %d\n", r->capacityDepends);
   printf("Action Capacity is %d\n", r->capacityActions);
   printf("Number of depends is %d\n", r->numDepends);
   printf("Number of actions is %d\n", r->numActions);
   for (i = 0; i < r->numDepends; i++) {
      printf("Depends[%d] = %s\n", i , r->depends[i]);
   }
   printf("\n");
   for (i = 0; i < r->numActions; i++) {
      printf("Actions[%d] = %s\n", i , r->actions[i]);
   }
}

int findRule (Rule **rulebook, char *name, int numRules) {//returns index of rule with same name as name
   int i;
   printf("Im going t search for the rule called ---%s---, there are %d rules\n", name, numRules);
   
   for (i = 0; i < numRules; i++) {
      if (strcmp(rulebook[i]->name, name) == 0)
      {
         return i;
      }
   }
   printf("Did not find rule so return -1\n");
   return -1;
}

int executeDepends(Rule **rules, int numRules, int indexRule, int indexDepends, int updated, int rootIndex) {//return 1 for updated
   printf("Executing rule name(%s) depend name(%s)\n\n", rules[indexRule]->name, rules[indexRule]->depends[indexDepends]);
   Rule *currRule = rules[indexRule];
   int i;
   time_t targetTime;
   int executeMe;
   int oupdated = updated;
   
   int temp;
   
   if (currRule->numDepends == 0) {//no depends so update!
      if (rootIndex != indexRule) {//run actions unless its root, in chich case we will perc it up
         printf("no dpeendss would do actions\n");
         for (i = 0; i < rules[indexRule]->numActions; i++) {
            printf("Executing command: %s\n", rules[indexRule]->actions[i]);
            system(rules[indexRule]->actions[i]);      
         }
      }
     return updated + 1;
   }
   
   struct stat bufT;
      if (stat(currRule->name, &bufT))
      {
         if (rootIndex != indexRule) {
            printf("No file with name of this target, would execute actions\n");
            executeMe = 1;
         } 
         updated++;
      }
   targetTime = bufT.st_mtime;   
   
   int validDepends = 0;
   temp = findRule(rules, currRule->depends[indexDepends], numRules);
   
   if (temp != -1) {//dependency is a rule
      validDepends = 1;
      updated += executeDepends(rules, numRules, temp, 0, updated, rootIndex); //needs to happen at least once
      for (i = 1; i < rules[temp]->numDepends; i++) {
         updated += executeDepends(rules, numRules, temp, i, updated, rootIndex);
         executeMe = 1;  
      }  
   }
   
   if (file_exist(currRule->depends[indexDepends])) { //dependency is a file
      validDepends = 1;
      struct stat buf;
      if (stat(currRule->depends[indexDepends], &buf))
      {
         printf("Error stating file\n");
         exit(-1);
      }
      if (difftime(buf.st_mtime, targetTime) > 0) {//File has been modified more recently than target
         updated++;
         executeMe = 1;
      } 
   }
   
   if (validDepends != 1) {
     printf("The depency is not valid\n");
     exit -1;
   }
   
   if (executeMe && indexRule != rootIndex) {
      printf("I should run actions for this rule named....%s so I will\n", rules[indexRule]->name);
      for (i = 0; i < rules[indexRule]->numActions; i++) {
         printf("Executing command: %s\n", rules[indexRule]->actions[i]);
         system(rules[indexRule]->actions[i]);      
      }
   }
   return updated; 
}
//Function for checking file exsistance found at 
   //http://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
int file_exist (char *filename)
{
   struct stat   buffer;   
   return (stat (filename, &buffer) == 0);
}