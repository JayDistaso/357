
#include "Calculon.h"

//static char *suiteFile;
static Suite suite;

int main(int argc, char **argv) {
   char *suiteFile;
   int numProgram;
   suiteFile = *(argv + 1);
   suite.programs = (Program **)calloc(MAX_FILES, sizeof(Program **));
   ParseSuite(suiteFile);
   //PrintSuite();
   for (numProgram = 0; numProgram < suite.numPrograms; numProgram++) {
      //printf("running program #%d\n", numProgram);
      RunProgram(numProgram);
   }
   return EXIT_SUCCESS;
}

void CopyToTemp(int numProg, char *tempDir) {
   int index; 
   Program *prog = suite.programs[numProg];
   Test *test;
   char *sourceDirectory = calloc(MAX_CHARS, sizeof(char));
   for (index = 0; index < prog->numInputFiles; index++) {
      if (access(prog->inputFiles[index], F_OK)) {
         getcwd(sourceDirectory, MAX_CHARS);
         printf("I am in path = %s\n", sourceDirectory);
         printf("Could not find required test file '%s'\n", prog->inputFiles[index]);
      }
      else {
         if (fork()) {
            wait(NULL);
         }
         else {
            
            execl(CP, CP, prog->inputFiles[index], tempDir, NULL);
            printf("BADFORK\n");  
         }
      }
   }
   for (index = 0; index < prog->numTests; index++) {
      test = prog->tests[index];
      if (access(test->inputFile, F_OK)) {
         getcwd(sourceDirectory, MAX_CHARS);
         printf("I2 am in path = %s\n", sourceDirectory);
         printf("Could not find required test file '%s'\n", test->inputFile);
         exit(1);
      }
      else {
         if (fork()) {
            wait(NULL);
         }
         else {
            execl(CP, CP, test->inputFile, tempDir, NULL);
                        printf("BADFORK2\n");
                                                exit(-1);  
  
  
         }
      }
      
      if (access(test->outputFile, F_OK)) {
         getcwd(sourceDirectory, MAX_CHARS);
         printf("I3 am in path = %s\n", sourceDirectory);
         printf("Could not find required test file '%s'\n", test->outputFile);
         exit(1);
      }
      else {
         if (fork()) {
            wait(NULL);
         }
         else {
            execl(CP, CP, test->outputFile, tempDir, NULL);
                        printf("BADFORK3\n");
                        exit(-1);  
  
         }
      }   
   }   
}

int CompileFile(int numProg, char *tempDir) {

   char **argv = calloc(MAX_FILES, sizeof(char *)), *extension;
   int argc = 0, index, status, success = 0;
   Program *prog = suite.programs[numProg];
   
   argv[argc++] = GCC;
   //argv[argc++] = prog->execName;
   
   for (index = 0; index < prog->numInputFiles; index++) {
      extension = strrchr(prog->inputFiles[index], '.');
      if (extension && !(strcmp(extension, ".c"))) {
         argv[argc++] = prog->inputFiles[index];
      }
   }
   
   argv[argc++] = "-o";
   argv[argc++] = prog->execName;
   argv[argc] = NULL;
   //printf("we generated a argv that looks like\n");
   //for (index = 0; index < argc + 1; index++) {
     // printf("argv[%d] = |%s|\n", index, argv[index]); 
   //}
   
   if (fork()) {
      wait(&status);
      if (WEXITSTATUS(status)) {
         printf("Failed: gcc");
         for (index = 1; index < argc; index++) {
            printf(" %s", argv[index]);
         }
         printf("\n");
         success = 0;
      }
      else {
         success = 1;
      }
   }
   else {
      chdir(tempDir);
      execv(GCC, argv);
      printf("BADFORK COMPILE\n");
      exit(-1); 
   }
   return success;      
}

int Makefile(int numProg, char *tempDir) {
   
   int success = 0, output, status;
   
   if (fork()) {
      wait(NULL);
   }
   else {
      execl(CP, CP, "Makefile", tempDir, NULL);
      printf("BADFORK4\n"); 
      exit(-1);  
 
   }
   
   if (fork()) {
      wait(&status);
      if (WEXITSTATUS(status) == 0) {
         success = 1; 
      }
      else {
         printf("Failed: make %s\n", suite.programs[numProg]->execName);
         success = 0;
      }
   }
   else {
      chdir(tempDir);
      output = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
      dup2(output, 1);
      close(output);
      execl(MAKE, MAKE, suite.programs[numProg]->execName, NULL);
                  printf("BADFORK5\n"); 
                                          exit(-1);  
 

   }
   printf("the make returned a status of %d\n", success);
   return success;   
}

Report *RunExec(Test *test, char **argv, char *tempDir) {
   
   int inputFile, outputFile, errorFile, status;
   Report *report = calloc(1, sizeof(Report));
   
   chdir(tempDir);
   inputFile = open(test->inputFile, O_RDONLY);
   outputFile = open("test.output.temp", O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
   errorFile = open("test.output.temp", O_WRONLY | PERMISSIONS);
   
   report->runtime = 0;
   report->timeout = 0;
   report->diffFail = 0;
   
   if (fork()) {
      close(inputFile);
      close(outputFile);
      close(errorFile);
      wait(&status);
      if (WEXITSTATUS(status) == RUNTIME_ERROR_CODE || WEXITSTATUS(status) == 1) {
         report->runtime = 1;
      }
      else if (WEXITSTATUS(status) == TIMEOUT_ERROR_CODE) {
         report->timeout = 1;
      }
      else if (WEXITSTATUS(status) == BOTH_ERROR_CODES) {
         report->timeout = 1;
         report->runtime = 1;
      }
      //free(argv);
   }
   else {
      chdir(tempDir);

      dup2(inputFile, 0);
      close(inputFile);

      dup2(outputFile, 1);
      close(outputFile);      
      
      dup2(errorFile, 2);
      close(errorFile);
 
      execv(SAFERUN, argv);
      printf("bad fork runexec\n");
      exit(-1);
   }
   return report;
}

int RunTest(int numProgram, int testNumber, char *tempDir) {
   
   Report *report;
   int argc = 0, index, testsFailed = 0;
   char *tFlag = calloc(MAX_CHARS, sizeof(char)), *scaledTFlag = calloc(MAX_CHARS, sizeof(char)), *execName = calloc(MAX_CHARS, sizeof(char)), **argv = calloc(MAX_CHARS, sizeof(char *));
   Program *prog = suite.programs[numProgram];
   Test *test = prog->tests[testNumber];
   
   sprintf(tFlag, "%s%d", "-t", test->time);
   sprintf(scaledTFlag, "%s%d", "-T", (test->time) * TSCALE);
   sprintf(execName, "%s%s", "./", prog->execName);
   
   argv[argc++] = SAFERUN;
   argv[argc++] = PFLAG;
   argv[argc++] = tFlag;
   argv[argc++] = scaledTFlag;
   argv[argc++] = execName;
   
   for (index = 0; index < test->argc; index++) {
      argv[argc++] = test->argv[index];
   }
   
   argv[argc] = NULL;
   report = RunExec(test, argv, tempDir);
   
   argc = 0;
   argv[argc++] = DIFF;
   argv[argc++] = test->outputFile;
   argv[argc++] = "test.output.temp";
   argv[argc] = NULL;
   
   RunDiff(argv, tempDir, report);
   testsFailed = CheckErrors(report, numProgram, testNumber);
   return testsFailed;
}

int CheckErrors(Report *report, int numProgram, int testNumber) {
   Program *prog = suite.programs[numProgram];
   int fail = 0;
   if (report->diffFail) {
      fail++;
   }
   if (report->runtime) {
      fail++;
   }
   if (report->timeout) {
      fail++;
   }
   if (fail) {
      //fail = 1;
      printf("%s test %d failed: ", prog->execName, testNumber);
      if (report->diffFail) {
         printf("diff failure");
         if (--fail > 1) {
            printf(", ");
         }
      }
      if (report->runtime) {
         printf("runtime error");
         if (--fail > 1) {
            printf(", ");
         }
      }
      if (report->runtime) {
         printf("timeout");
      }
      printf("\n");
      fail = 1;
   }
   return fail;
}

void RunDiff(char **argv, char *tempDir, Report *report) {
   
   int status, outputFile;
   
   if (fork()) {
      wait(&status);
      if (WEXITSTATUS(status) != 0) { //Differences found
         report->diffFail = 1;
      }//ask about diff error    
   }
   else {
      chdir(tempDir);
      outputFile = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
      dup2(outputFile, 1);
      close(outputFile);
      execv(DIFF, argv); 
   }
}

void RunProgram(int numProgram) {
   int pid, success, index, failed = 0;
   char *tempDirectory = calloc(MAX_CHARS, sizeof(char)), *sourceDirectory = calloc(MAX_CHARS, sizeof(char));
   Program *prog = suite.programs[numProgram];
   pid = getpid();
   sprintf(tempDirectory, "%s%d", ".", pid);
   mkdir(tempDirectory, S_IRWXU);
   
   getcwd(sourceDirectory, MAX_CHARS);
   printf("the source dir is %s and the temp dir is %s\n", tempDirectory, sourceDirectory);
   CopyToTemp(numProgram, tempDirectory);
   if (access("Makefile", F_OK)) {
      success = CompileFile(numProgram, tempDirectory);
   }
   else {
      success = Makefile(numProgram, tempDirectory);
   }
   if (success) {
      for (index = 0; index < prog->numTests; index++) {
         failed += RunTest(numProgram, index, tempDirectory);
      }
   }
   if (!(failed)) {
      printf("%s %d of %d tests passed.\n", prog->execName, prog->numTests, prog->numTests);
   }
   chdir(sourceDirectory);
   //getcwd(sourceDirectory, MAX_CHARS);
   //printf("about to delete directory named |%s| in |%s|\n", tempDirectory, sourceDirectory);
   DeleteDirectory(tempDirectory);
}

void DeleteDirectory(char *name) {
   DIR *directory = opendir(name);
   struct dirent *directoryEntry;
   char *removeName = calloc(MAX_CHARS, sizeof(char));

   if (directory) {
      while ((directoryEntry = readdir(directory))) {
         sprintf(removeName, "%s/%s", name, directoryEntry->d_name);
         remove(removeName);
      }
      rmdir(name);
   }
   else {
      printf("that directory did not exsist?\n");
   }
   
}

void ParseSuite(char *fileName) {
   
   FILE *file;
   char *line = calloc(MAX_CHARS, sizeof(char)), *letter = calloc(2, sizeof(char));
   int numTests = 0;
   Program *program;
   Test *test;
   
   file = fopen(fileName, "r");

   if (!file) {
      printf("failed to open file with name -%s-\n", fileName);
      exit(-1);
   }

   while (fgets(line, MAX_CHARS, file)) {
      sscanf(line, "%c", letter);
      if (strcmp(letter, "P") == 0) {
         program = ParseProgram(line);
         
         (suite.programs)[suite.numPrograms] = calloc(1, sizeof(Program *));
         (suite.programs)[(suite.numPrograms)] = program;
         (suite.programs)[suite.numPrograms++]->tests = (Test **)calloc(MAX_FILES, sizeof(Test **));

      }
      else if(strcmp(letter, "T") == 0) {
         test = ParseTest(line);
         numTests = ((suite.programs)[suite.numPrograms - 1])->numTests++;
         (((suite.programs)[suite.numPrograms - 1])->tests)[numTests] = (Test *)calloc(1, sizeof(Test *));
         ((suite.programs)[suite.numPrograms - 1])->tests[numTests] = test;         
      }
      //no argument? 
   }
}

Program *ParseProgram(char *line) {
   char *token;
   int numInputFiles = 0;
   Program *program = calloc(1, sizeof(Program));
   
   program->inputFiles = calloc(MAX_FILES, sizeof(char *));
   token = strtok(line, " \n");
   token = strtok(NULL, " \n");
   program->execName = strdup(token);
   while ((token = strtok(NULL, " \n"))) {
      program->inputFiles[numInputFiles++] = strdup(token);
   }
   program->numInputFiles = numInputFiles;
   return program;
}

Test *ParseTest(char *line) {
   
   char **argv = calloc(MAX_FILES, sizeof(char *)), *token;
   int argc = 0;
   Test *test = calloc(1, sizeof(Test));
   test->argv = argv;
   
   token = strtok(line, " \n");
   token = strtok(NULL, " \n");
   test->inputFile = strdup(token);
   
   token = strtok(NULL, " \n");
   test->outputFile = strdup(token);

   token = strtok(NULL, " \n");
   test->time = atoi(token);
   
   while ((token = strtok(NULL, " \n"))) {
      test->argv[argc++] = strdup(token);
   }
   test->argc = argc;
   return test;
}

void PrintSuite() {
   int i, j, k;
   Program *p;
   Test *t;
   printf("printing suite with %d progs\n", suite.numPrograms);
   for (i = 0; i < suite.numPrograms; i++) {
      p = suite.programs[i];
      printf("\nprogram %d\n", i);
      printf("exec name = %s\n", p->execName);
      printf("numInptFiles = %d\n", p->numInputFiles);
      for (k = 0; k < p->numInputFiles; k++) {
         printf("InptFiles %d = %s\n", k, p->inputFiles[k]);
         
      }
      printf("numTests = %d\n", p->numTests);
      printf("Printing tests:\n");
      for (j = 0; j < p->numTests; j++) {
         t = p->tests[j];
         printf("test #%d\n", j);
         printf("input file = %s\n", t->inputFile);
         printf("output file = %s\n", t->outputFile);
         printf("argc = %d\n", t->argc);
         for (k = 0; k < t->argc; k++) {
            printf("argv%d  = %s\n", k, t->argv[k]);
         }
         printf("time = %d\n", t->time);
      }
   }
   printf("done printing\n");
}