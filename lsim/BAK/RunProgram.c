#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "List.h"
#include "RunProgram.h"
#include "RunTest.h"

#define MAX_CHARS 256
#define MAX_ARGS 36
#define CHARS 6
#define CP "/bin/cp"
#define GCC "/usr/bin/gcc"
#define MAKE "/usr/bin/make"
#define PERMISSIONS 0644

#define EXEC_NAME "Calculon"

void CopyFile(char filename[], char tempDir[]) {
   if (fork()) {
      wait(NULL);
   }

   else {
      execl(CP, CP, filename, tempDir, NULL);
   }
}

int DirEmpty(const char *path) {
   struct dirent *ent;
   int ret = 1;
 
   DIR *dp1 = opendir(path);
   
   if (!dp1) {
      fprintf(stderr, "%s: ", path);
      perror("");
      return -1;
   }
   
   while ((ent = readdir(dp1))) {
      if (!strcmp(ent->d_name, ".") || !(strcmp(ent->d_name, "..")))
         continue;
      ret = 0;
      break;
   }
 
   closedir(dp1);
   return ret;
}

void DeleteDir(char *dir, int status) {
   struct dirent *dirEnt;
   DIR *dp1;
   char filename[MAX_CHARS];
   
   if (status) {
      chdir("../");
   }
   
   dp1 = opendir(dir);
   dirEnt = readdir(dp1);
 
   while (dirEnt) {
      sprintf(filename, "%s/%s", dir, dirEnt->d_name); 
      remove(filename);
      dirEnt = readdir(dp1);
   }
   if (DirEmpty(dir)) {
      rmdir(dir);
   }
}

int HasFile(char *filename) {
   if (access(filename, F_OK) != -1) {
      return 1;
   }

   return 0;
}

void CopyAllFiles(Program *prog, char *dirTarget) {
   Test *test;
   int ndx;
   
   CopyFile(prog->progName, dirTarget);
   for (ndx = 0; ndx < prog->numInFiles; ndx++) {
      if (HasFile(prog->inFiles[ndx])) {
         CopyFile(prog->inFiles[ndx], dirTarget);
      }

      else {
         printf("Could not find required test file '%s'\n", prog->inFiles[ndx]);
      }
   }
   
   test = prog->head;
   while (test) {
      if (HasFile(test->inFile)) {
         CopyFile(test->inFile, dirTarget);
      }

      else {
         printf("Could not find required test file '%s'\n", test->inFile);
         exit(EXIT_FAILURE);
      }
      
      if (HasFile(test->outFile)) {
         CopyFile(test->outFile, dirTarget);
      }

      else {
         printf("Could not find required test file '%s'\n", test->outFile);
      }
      
      test = test->next;
   }
}
 
int IsCFile(char *filename) {
   const char *dot = strrchr(filename, '.');
   
   if (!dot) {
      return 0;
   }

   if (strcmp(dot, ".c") != 0) {
      return 0;
   }
  
   else {
      return 1;
   }
}

int Compile(Program *prog, char *tempDir) {
   char **argv = calloc(MAX_ARGS, sizeof(char *));
   int ndx, status, argc = 0, success;
   
   argv[argc++] = GCC;
   argv[argc++] = prog->progName;
   
   for (ndx = 0; ndx < prog->numInFiles; ndx++) {
      if (IsCFile(prog->inFiles[ndx])) {
         argv[argc++] = prog->inFiles[ndx];
      }
   }
   argv[argc++] = "-o";
   argv[argc++] = prog->execName;
   argv[argc] = NULL;

   if (fork()) {
      wait(&status);
      if (WEXITSTATUS(status) != 0) {
         printf("Failed: gcc");
         for (ndx = 1; ndx < argc; ndx++) {
            printf(" %s", argv[ndx]);
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
   }
   return success;
}
    

int Make(Program *prog, char *tempDir) {
   int status, out, success = 0;
   
   CopyFile("Makefile", tempDir);
    
   if (fork()) {
      wait(&status);
      if (WEXITSTATUS(status) == 0) {
         success = 1; 
      }
      else {
         printf("Failed: make %s\n", prog->execName);
         success = 0;
      }
   }

   else {
      chdir(tempDir);
      out = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
      dup2(out, 1);
      close(out);
      execl(MAKE, MAKE, prog->execName, NULL);
   }
   return success;
}

void CheckTest(char *execName, int numTests, int passed) {
   if (passed == numTests) {
      printf("%s %d of %d tests passed.\n", execName, passed, numTests);
   }
}

int RunProgram(Program *prog, char *suiteFile) {
   int pid, testNum = 0, passed = 0, status = 0;
   char tempDir[MAX_CHARS], sourceDir[MAX_CHARS];
   Test *test; 
   
   pid = getpid();
   sprintf(tempDir, "%s%d", ".", pid);
   mkdir(tempDir, S_IRWXU);
     
   getcwd(sourceDir, MAX_CHARS);
   CopyAllFiles(prog, tempDir);
   
   if (!HasFile("Makefile")) {
      status = Compile(prog, tempDir);     
   }
   
   else {
      status = Make(prog, tempDir);
   }
   
   if (status) {
      test = prog->head;
      while (test) {
         passed +=  RunTest(test, tempDir, prog->execName, testNum);
         test = test->next;
         testNum++;
      }
      CheckTest(prog->execName, testNum, passed);
   }
   
   DeleteDir(tempDir, status);
   return EXIT_SUCCESS;
}
