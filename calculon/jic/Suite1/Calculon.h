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


#define MAX_CHARS 256
#define MAX_FILES 32
#define PERMISSIONS 0777
#define TSCALE 10
#define RUNTIME_ERROR_CODE 200
#define TIMEOUT_ERROR_CODE 193
#define BOTH_ERROR_CODES 201

#define PFLAG "-p30"
#define CP "/bin/cp"
#define GCC "/usr/bin/gcc"
#define MAKE "/usr/bin/make"
#define DIFF "/usr/bin/diff"
//#define SAFERUN "/home/grade-cstaley/bin/SafeRun"
#define SAFERUN "/home/jdistaso/357/calculon/test/SafeRun"

typedef struct Test {
   char *inputFile;
   char *outputFile;
   int time;
   char **argv;
   int argc;
} Test;

typedef struct Program {
   Test **tests;
   char *execName;
   //char *progName;
   char **inputFiles;
   int numInputFiles;
   int numTests;
} Program;

typedef struct Suite {
   Program **programs;
   int numPrograms;
} Suite;

typedef struct Report {
   int timeout;
   int diffFail;
   int runtime;
} Report;

void PrintSuite();
void ParseSuite(char *fileName);
Test *ParseTest(char *line);
Program *ParseProgram(char *line);
void DeleteDirectory(char *name);
void CopyToTemp(int numProg, char *tempDir);
int Makefile(int numProg, char *tempDir);
void RunProgram(int numProgram);
int CompileFile(int numProg, char *tempDir);
Report *RunExec(Test *test, char **argv, char *tempDir);
void RunDiff(char **argv, char *tempDir, Report *report);
int CheckErrors(Report *report, int numProgram, int testNumber);
int RunTest(int numProgram, int testNumber, char *tempDir);

