/*
 * Copyright Software Innovations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UINT_SIZE 32
#define UINT_MASK 0xFFFFFFFF

typedef unsigned int UInt;

typedef struct {
   UInt curData;
   UInt nextData;
   int bitsLeft;
   int validNext;
} BitUnpacker;

//BuInit



//BuTakeData



/*BuUnpack
 * return 1 on successful, 0 for needing next block
 */



int main() {
   BitUnpacker bu;
   int numBlocks, size, i;
   UInt ret, *temp;

   scanf("%d", &numBlocks);
   temp = calloc(sizeof(UInt), numBlocks);

   for (i = 0; i < numBlocks; i++) {
      scanf(" %x", &temp[i]);
   }

   BuInit(&bu);
   i = 0;

   while (scanf(" %d", &size) != EOF) {
      if (!BuUnpack(&bu, size, &ret)) {
         if (i < numBlocks) {
            BuTakeData(&bu, temp[i++]);
            BuUnpack(&bu, size, &ret);
         }
         else {
            printf("Not enough data left.\n");
         }
      }
      printf("curData is %X (Should be unchanged until no bitsLeft)\n",
       bu.curData);
      printf("data: %08X\n", ret);
   }
   printf("\n");
   free(temp);

   return 0;
}