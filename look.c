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

//statics used for commandline parseing
static int dFlag = 0, tFlag = 0, fFlag = 0;
static char *tArg = NULL, *fileArg = NULL, *stringArg = NULL;

int main(int argc, char **argv) {
   GetFlags(argc, argv);
   /* f erease case from string / file on by default for no file*/
   /* d ignores non-alhha numeric from string / file on if no file*/
   /* t trims string */
   printf("Flag status d%d f%d t%d\n", dFlag, fFlag, tFlag);
   printf("tArg |%s|, fileArg |%s|, stringArg|%s|\n", tArg, fileArg, stringArg); 
   //returns 0 for one or more lines, 1 for no lines, >1 for error
   
}

//getopt code adapted from 
//http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
void GetFlags(int argc, char **argv) {
   
   int optVal, argsLeft;
   
   while ((optVal = getopt(argc, argv, "dft:"))) { //add : here?
      switch (optVal) {
      
      case 'd':
         dFlag = 1;
      break;
      
      case 't':
         tFlag = 1;
         tArg = optarg;
      break;
      
      case 'f':
         fFlag = 1;
      break;
      
      case '?':
         fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);

      default:
         assert(0);
      }
   }
   
   argsLeft = optind - argc;
   
   if (argsLeft > 2 || argsLeft < 1) {
      printf("Too few/many arguments\n");
      exit(2); 
   }
   else if (argsLeft == 2) {
      stringArg = argv[optind];
      fileArg = argv[++optind];
   }
   else if (argsLeft == 1) {
      stringArg = argv[optind];
   }
   else {
      assert(0);
   }
   if (!fileArg) { //On by default for no file
      dFlag = 1;
      fFlag = 1;
   }
   
   
}