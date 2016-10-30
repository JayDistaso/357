#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Report.h"

#define MAX_CHARS 100
#define EXEC_NAME "./Cell"
#define ONE_INNER_CELL 3
#define TWO_INNER_CELLS 4
#define MANY_CELLS 5
#define BASE 10
#define MAX_FD 12
#define READ 0
#define WRITE 1

void InputErrorCheck(int numCells, int rightSet, int simTimeTaken,
int cellNumberTaken, int originalArgc);

void BranchCase(int numCells, int simTime, double rightVal, 
 double leftVal, int parentPipe[2]);
 
void OneCell(int parentPipe[2], char *simTimeString,
 char *parentPipeWriteString, char *leftValString, int numCells,
 int simTime);
 
void ReadReports(int numCells, int simTime, int parentPipe[2]);


int main(int argc, char **argv) {
   
   int numCells = 0, numCellsTaken = 0, simTimeTaken = 0,
    simTime, rightSet = 0, leftSet = 0, originalArgc,
    *reports, *pids, parentPipe[2];
   
   double value = 0, leftVal = 0, rightVal = 0;
   
   char *end;
   
   originalArgc = argc;
   
   //Parse arguments
   while(--argc) {
      if (**(++argv) == 'C' && numCellsTaken == 0) {
         value = strtol(*argv + 1, &end, BASE);
         if (*end == '\0' && (numCellsTaken = 1)) {
            numCells = value;
         }
      }
      else if (**(++argv) == 'S' && simTimeTaken == 0) {
         value = strtol(*argv + 1, &end, BASE);
         if (*end == '\0' && (simTimeTaken = 1)) {
            simTime = value;
         }
      }
      else if (**(++argv) == 'L' && leftSet == 0) {
         value = strtod(*argv + 1, &end);
         if (*end == '\0' && (leftSet = 1)) {
            leftVal = value;
         }
      }
      else if (**(++argv) == 'R' && rightSet == 0) {
         value = strtod(*argv + 1, &end);
         if (*end == '\0' && (rightSet = 1)) {
            rightVal = value;
         }
      }
   }
   
   InputErrorCheck(numCells, leftVal, rightSet, rightVal, simTimeTaken,
    cellNumberTaken, originalArgc);
   
   
   //pids = calloc(numCells, sizeof(int));
   //reports = calloc(numCells, sizeof(int));
   pipe(parentPipe); //0 is read end, 1 is write end
   
   BranchCase(numCells, simTime, rightVal, leftVal, parentPipe);
      
}

void ReadReports(int numCells, int simTime, int parentPipe[2]) {
    
   int data, report;
   
   Report reportIn;
   for (report = 0; report < numCells * (simTime + 1); report++) {  
      data = read(parentPipe[READ], &reportIn, sizeof(Report));
         
      if (data < 0) {
         exit(EXIT_FAILURE);
      }
   
      printf("Result from %d, step %d, %lf\n", reportIn.id, reportIn.step, reportIn.value);
   }

}

void BranchCase(int numCells, int simTime, double rightVal, 
 double leftVal, int parentPipe[2]) {
   
   char *simTimeString, *parentPipeWriteString, leftValString, rightValString;
   
   sprintf(simTimeString, "%s%d", "S", simTime);
   sprintf(parentPipeWriteString, "%s%d", "O", parentPipe[WRITE]);
   sprintf(leftValString, "%s%lf", "V", leftVal);
   sprintf(rightValString, "%s%lf", "V", rightVal);
   
   if (numCells == 1) {
      OneCell(simTimeString, parentPipeWriteString, leftValString);
   }
   else if(numCells == 2) {
      //TwoCells(
   }
   else if(numCells == ONE_INNER_CELL) {
      //OneInnerCell
   }
   else if(numCells == TWO_INNER_CELLS) {
      //TwoInnerCells(
   }
   else if(numCells >= MANY_CELLS) {
      //ManyCells(
   }

}

void OneCell(int parentPipe[2], char *simTimeString,
 char *parentPipeWriteString, char *leftValString, int numCells,
 int simTime) {
   
   int *status, childPID;
   
   if ((childPID = fork()) < 0) {
      fprintf(stderr, "Bad fork call\n"); //or other error
   }
   else if (childPID > 0) {//Parent Code
      close(parentPipe[WRITE]);
      ReadReports(numCells, simTime, parentPipe[2]);      
   }
   else {//Child code
      close(parentPipe[READ]);
      execl("./Cell", simTimeString, parentPipeWriteString, "D1", leftValString, "\0");
   }
   wait(&status);
   CheckStatus(status);
}

void CheckStatus(int *status) {
   //check the status and throw an error or something
   if (*status != 0) {
      printf("exited with non-zero status\n");
      exit(-1);
   }
}


void InputErrorCheck(int numCells, int rightSet, int simTimeTaken,
 int cellNumberTaken, int originalArgc) {
 
   if ((rightSet && numCells) < 2 || !cellNumberTaken || !simTimeTaken) {
      fprintf(stderr, "Usage: LinearSim C S L R (in any order)\n");
      exit(EXIT_FAILURE);
   }

}
   