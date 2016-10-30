ccccc/*
 * Copyright Software Inventions 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SmartAlloc.h" // #include "SmartAlloc.h" if not on IHS

#define UINT_SIZE 32

typedef unsigned int UInt;

typedef struct {
   UInt *data;  // NULL or points to 1 or 2 allocated UInts
   int count;   // 0 to 2
} IntBlock;

typedef struct {
   UInt packedData;
   int bitsUsed;
} BitPacker;


//BpInit

void BpInit(BitPacker *bp) { // Base: 3, Surcharge: 0
   bp->packedData = 0;       // Base: 3, Surcharge: 0
   bp->bitsUsed = 0;         // Base: 3, Surcharge: 0
}                            



//BpPack

IntBlock BpPack(BitPacker *bp, UInt data, int size, int last) {                 // Base: 6, Surcharge: 0
   IntBlock ret;                                                              
   ret.count = 0;                                 // Base: 3, Surcharge: 0
   ret.data = calloc(2, sizeof(UInt));                                         // Base: 7, Surcharge: 0
   int left = 0;                                                                // Base: 3, Surcharge: 0
   UInt temp;                                                                 
   if (bp->bitsUsed + size <= UINT_SIZE) { //data fits                          // Base: 7, Surcharge: 0                                                     // Base: 4, Surcharge: 0
      bp->packedData |= (data << (UINT_SIZE - size - bp->bitsUsed));          // Base: 9, Surcharge: 0             // Base: 5, Surcharge: 0
      bp->bitsUsed += size;
      printf("bp->bitsUsed changed to %d\n", bp->bitsUsed);              // Base: 3, Surcharge: 0
      if (bp->bitsUsed == UINT_SIZE) {                                   // Base: 5, Surcharge: 0
         last = 1;
         printf("I filled a block, print me?\n");                                                              // Base: 3, Surcharge: 0
      }                                                                       
   }                                                                          
   else { //data dose not fit                                                   // Base: 1, Surcharge: 0                                                  // Base: 4, Surcharge: 0
      bp->packedData |= (data >> (left = (size + bp->bitsUsed) - UINT_SIZE)); // Base: 14, Surcharge: 0//fill pack                                          // Base: 5, Surcharge: 0                  // Base: 5, Surcharge: 0
      UInt extradata;                                                         
      temp = (data & ((1 << left) - 1)) << (UINT_SIZE - left);
      bp->bitsUsed = UINT_SIZE;
      if (last == 0) {
         ret.count = 1;
         ret.data[0] = bp->packedData;
         bp->packedData = temp;
         bp->bitsUsed = left;  
      }                // Base: 13, Surcharge: 0                 // Base: 3, Surcharge: 0
   }                                                                          
   if (last) {                                                                  // Base: 3, Surcharge: 0                                                          // Base: 4, Surcharge: 0
      ret.count = 1;                                                          // Base: 3, Surcharge: 0
      if (left) {                                                               // Base: 3, Surcharge: 0
         ret.count = 2;                                                        // Base: 3, Surcharge: 0
         ret.data[1] = temp;                                                        // Base: 5, Surcharge: 0
      }                                                                       
      ret.data[0] = bp->packedData;                                            // Base: 5, Surcharge: 0
      BpInit(bp);                                           // Base: 3, Surcharge: 0                                                         // Base: 3, Surcharge: 0
   }                                                                          
   return ret;                                                                 // Base: 1, Surcharge: 0
}                                                                         



int main() {
   BitPacker bp;
   IntBlock iB;
   UInt data;
   int size, count, idx;

   scanf("%d", &count);

   BpInit(&bp);
   while (count--) {
      scanf("%u %d", &data, &size);
      iB = BpPack(&bp, data, size, count == 0);
      for (idx = 0; idx < iB.count; idx++) {
         printf("%x ", *(iB.data + idx));
      }
      if (iB.data) {
         free(iB.data);
      }
   }
   printf("\n");
   printf("Allocated space left: %ld\n", report_space());
   return 0;
}