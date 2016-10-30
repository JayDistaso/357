#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Report.h"
#define MAX_FD 12
#define BASE 10

void InitValues(int *simTime, int *outputs, int *inputs,
 int *cellID, double *fixedVal, int *fixed, int *outFD[MAX_FD], 
 int *inFD[MAX_FD], char **argv, int argc);

void Simulate(int simTime, int outputs, int inputs, int cellID,
 double fixedVal, int fixed, int *outFD, int *inFD);

int main(int argc, char **argv) {

   int simTime, outputs = 0, inputs = 0, cellID, fixed = 0;
   double fixedVal = 0;
   int *outFD = (int *)calloc(MAX_FD, sizeof(int)),
    *inFD = (int *)calloc(MAX_FD, sizeof(int));
   
   
   InitValues(&simTime, &outputs, &inputs, &cellID,
    &fixedVal, &fixed, &outFD, &inFD, argv, argc);

   Simulate(simTime, outputs, inputs, cellID, fixedVal, fixed,
    outFD, inFD); 
     
   exit(EXIT_SUCCESS);   
}

void Simulate(int simTime, int outputs, int inputs, int cellID, double
 fixedVal, int fixed, int *outFD, int *inFD) {

   int step, index, numCells = 0, data; 
   double sum = 0, average = 0;
 
   Report reportIn, reportOut;
 
   for (step = 0; step <= simTime; step++) {
      
      for (index = 0; index < outputs; index++) {         
         
         reportOut.id = cellID;
         reportOut.step = step;
         
         if (fixed) {
            reportOut.value = fixedVal;
         }
         else {
            reportOut.value = average;
         }
         data = write(outFD[index], &reportOut, sizeof(Report));
 
         if (data < 0) {
            exit(EXIT_FAILURE); 
         }
      }
      
      for (index = 0; index < inputs; index++) {
         data = read(inFD[index], &reportIn, sizeof(Report));
         
         if (data < 0) {
            exit(EXIT_FAILURE);
         }
         
         sum += reportIn.value;
         numCells++;
      }
      
      if (numCells) {
         average = sum / numCells;
      }
      numCells = 0;
      sum = 0;
   }
   for (index = 0; index < outputs; index++) {
      close(outFD[index]);
   }
   
   for (index = 0; index < inputs; index++) {
      close(inFD[index]);
   }
   exit(EXIT_SUCCESS); 
}

void InitValues(int *simTime, int *outputs, int *inputs,
 int *cellID, double *fixedVal, int *fixed, int **outFD, 
 int **inFD, char **argv, int argc) {
   
   double value;
   char *end;

   while (argc--) {
      if (**argv == 'S') {
         value = strtol(*argv + 1, &end, BASE);
         if (*end == '\0') {
            *simTime = value;
         }
      }
      else if (**argv == 'O') {
         value = strtol(*argv + 1, &end, BASE);
         
         if (*end == '\0' && value <= MAX_FD) {
            (*outFD)[(*outputs)++] = value;
         }
         else if (value > MAX_FD) {
            fprintf(stderr, "OUTPUT ERROR");
            exit(EXIT_FAILURE);
         }
      }
      else if (**argv == 'I') {
         value = strtol(*argv + 1, &end, BASE);
         if (*end == '\0' && value <= MAX_FD) {
            (*inFD)[(*inputs)++] = value;
         }
         else if (value > MAX_FD) {
            fprintf(stderr, "INPUT ERROR\n");
            exit(EXIT_FAILURE);
         }
      }
      else if (**argv == 'D') {
         value = strtol(*argv + 1, &end, BASE);
         if (*end == '\0') {
            *cellID = value;
         }
      }
      else if (**argv == 'V') {
         value = strtod(*argv + 1, &end);
         if (*end == '\0') {
            *fixed = 1;
            *fixedVal = value;
         }
      }
      argv++;
   }
}