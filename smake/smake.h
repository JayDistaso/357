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