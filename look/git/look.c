#include "look.h"

//statics used for commandline parseing
static int DFlag = 0, TFlag = 0, FFlag = 0;
static char TArg, *FileArg = NULL, *StringArg = NULL;

int main(int argc, char **argv) {
   
   int lookVal = 0;
   
   GetFlags(argc, argv);
   TrimString();
   StringArg = StripChars(StringArg);
   lookVal = Look();
   
   return (!lookVal);
}

char *GetLine(FILE *file) {
   char *buffer = calloc(MAX_LINE_LENGTH, sizeof(char));
   char character;
   
   while (ftell(file) != 0) {
      character = fgetc(file);
      fseek(file, -1, SEEK_CUR);
      if (character == '\n') {
         fseek(file, 1, SEEK_CUR);
         fgets(buffer, MAX_LINE_LENGTH, file);
         break;
      }
      else {
         fseek(file, -1, SEEK_CUR);
      }
   }
   if (ftell(file) == 0) {
      fgets(buffer, MAX_LINE_LENGTH, file);
   }
   return buffer;
}

//returns 0 for match (go back first part)
// >0 for line is greater than string (go back in dict)
// <0 for line is less than string (go fwd in dict)
int CompareLine(char *line) {
   
   int stringIndex = 0, lineIndex = 0;
   char fromString = StringArg[stringIndex++], fromLine,
    *originalLine = strdup(line);
   
   originalLine = StripChars(originalLine);
   originalLine = strtok(originalLine, " \n");
   fromLine = originalLine[lineIndex++];
   
   while (fromString != '\0' && fromLine != '\0') {
      if (FFlag) {
         fromString = tolower(fromString);
         fromLine = tolower(fromLine); 
      }
      
      if (fromString != fromLine) {
         free(originalLine);
         return fromLine - fromString;
      }
      else {
         fromString = StringArg[stringIndex++];
         fromLine = originalLine[lineIndex++];
      }
   }
   
   free(originalLine);
   
   if (fromString == '\0') {
      return 0;
   }
   
   return -1;
}

int Look() {
   
   int match = 0, lineCount = 0, error = 0;
   long fileEnd = 0, filePosition = 0;
   char *line;
   FILE *file = NULL;
   
   if (FileArg) {
      file = fopen(FileArg, "r");
   }
   else {
      file = fopen(UNIX_DICT, "r");
   }
   
   if (file == NULL) {
      printf("look: %s: No such file or directory\n", FileArg);
      return 2; 
   }
   
   error = fseek(file, 0, SEEK_END); // go to end
   fileEnd = ftell(file);
   error = fseek(file, ftell(file) / 2, SEEK_SET); // go to middle
   line = GetLine(file);
   match = CompareLine(line);
   
   if (match >= 0) {//start searching backwards binary search
      while (match >= 0 && ftell(file) != 0) {
      //this loop will exit when we need to go fwd in dict, or we have reached start of file
         
         fseek(file, (ftell(file) - 1) / 2, SEEK_SET); //jump halfway back in file
         line = GetLine(file);
         match = CompareLine(line);
         
         if (ftell(file) < MAX_WORD_SIZE) { 
         //This conditional prevents first word edge case
    
            rewind(file);
            fseek(file, 2, SEEK_SET);                     
            break;
         }
      }
   }
   else {
      while (match < 0 && ftell(file) < fileEnd) {//start searching forwards binary search
         filePosition = ftell(file);
         fseek(file, ((fileEnd - ftell(file)) / 2) + ftell(file), SEEK_SET); 
         line = GetLine(file);
         match = CompareLine(line);
      } 
      fseek(file, filePosition, SEEK_SET); //set it backwards a step so we havent gone too far
   }
   
   match = -1;
   filePosition = -1;
   while (match <= 0 && ftell(file) < fileEnd) {
      fseek(file, -1, SEEK_SET);
      
      if (match == 0) {
         printf("%s", line);
         lineCount++;
      }
      
      line = GetLine(file);
      
      if (filePosition == ftell(file)) {//already checked this one
         break;
      }
      
      filePosition = ftell(file);
      match = CompareLine(line);
   }
   
   if (ftell(file) == fileEnd) {//edge case for checking end of file
      if (match == 0) {
         printf("%s", line);
         lineCount++;
      }
   }
   return lineCount; //This is the number of lines that match string
}

//This function strips out non-alpha numeric chars from a string if flag is set
char *StripChars(char *string) {
   
   int oldIndex = 0, newIndex = 0;
   
   if (DFlag) {
      while (string[oldIndex] != '\0') {
         if (isalnum(string[oldIndex])) {
            string[newIndex++] = string[oldIndex];
         }
         oldIndex++;
      }
      string[newIndex] = '\0';   
   }
   return string;
}

//This function inserts a null char after terminating char is found in string if t flag is set
void TrimString() {
   
   int index = -1;
   
   if (TFlag) {
      while ((char)StringArg[++index] != '\0') {
         if (StringArg[index] == TArg) {
            StringArg[index + 1] = '\0'; 
         }
      }
   }
}

//getopt code adapted from 
//http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
void GetFlags(int argc, char **argv) {
   
   int optVal, argsLeft;
   
   while ((optVal = getopt(argc, argv, "dft:")) != -1) {
      
      switch (optVal) {
      
      case 'd':
         DFlag = 1;
         break;
      
      case 't':
         TFlag = 1;
         TArg = optarg[0];
         break;
      
      case 'f':
         FFlag = 1;
         break;
      
      case '?':
         fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
         break;
      
      default:
         break;
      }
   }
   
   argsLeft = argc - optind;

   if (argsLeft > 2 || argsLeft < 1) {
      printf("Too few/many arguments\n");
      exit(2); 
   }
   else if (argsLeft == 2) {
      StringArg = argv[optind];
      FileArg = argv[++optind];
   }
   else if (argsLeft == 1) {
      StringArg = argv[optind];
   }
   else {
      assert(0);
   }
   
   if (!FileArg) { //On by default for no file
      DFlag = 1;
      FFlag = 1;
   }  
}