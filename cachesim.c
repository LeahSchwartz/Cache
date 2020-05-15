#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int myPow(int x,int n)
{
  int i; /* Variable used in loop counter */
  int number = 1;

  for (i = 0; i < n; ++i)
      number *= x;

  return(number);
}

int log2(int n) {
  int r=0;
  while (n>>=1) r++;
  return r;
}

struct frame{
  int recentNum;
  int valid;
  int tag;
};


int cacheSim(FILE* loadStoreFile, int cacheSize, int associativity, int blockSize){

  int DEBUG = 0;

  char operation[100];
  int compareToStore = -1;

  int cacheSizeinBytes = cacheSize * 1024;
  int numFrames = cacheSizeinBytes / blockSize;
  int numSets = numFrames / associativity;

  //int* mainMemory = (int*) malloc (2* sizeof(int) * myPow(2,24));
  char* mainMemory = (char*) malloc (sizeof(char) * myPow(2,24));
  char* mainMemoryTwo = (char*) malloc (sizeof(char) * myPow(2,24));
  char zeroString[2] = "0";
  char zeroChar = zeroString[0];
  // memset(mainMemory, zeroChar, sizeof(char) * myPow(2,24));
  // memset(mainMemoryTwo, zeroChar, sizeof(char) * myPow(2,24));
  // for (int k = 0; k < myPow(2,24); k++){
  //   mainMemory[k] = zeroChar;
  //   mainMemoryTwo[k] = zeroChar;
  //   // strcpy(mainMemory[k], "0");
  // }
  struct frame myCache[numSets][associativity];
  int numSetBits = log2(numSets);
  int numOffsetBits = log2(blockSize);

  for (int l = 0; l < numSets; l++){
    for (int k = 0; k < associativity; k++){
      myCache[l][k].recentNum = -1;
      myCache[l][k].valid = 0;
      myCache[l][k].tag = -1;
    }
  }

  int counter = 0;
  if (DEBUG == 1){

    printf("%s %d\n", "cache size in bytes =",cacheSizeinBytes);
    printf("%s %d\n", "number of blocks =",numFrames);
    printf("%s %d\n", "number of sets =",numSets);

  }

if (DEBUG == 1){
for (int g = 0; g < 10; g++){
  for (int j = 0; j < associativity; j++){ // figure out if hit or miss based on if tag is there
      printf("%s %d", "data for set:", g);
      printf("%s %d %s %d %s %d\n", "tag bits for way", j, "are:", myCache[2][j].tag, "and valid bits are:", myCache[2][j].valid);
    }
  }
}


  while (fscanf(loadStoreFile, " %s", &operation) == 1){
    DEBUG = 0;

    //printf("%s %s\n","operation:", operation);
    //printf("%s\n", "in While");
    //fscanf(loadStoreFile, "%s", &operation);
    //printf("%s %s\n","operation:", operation);
    char storeStr[6] = "store";
    compareToStore = strcmp(operation, storeStr);
    //printf("%s %d\n", "compare to store =",compareToStore);

    printf("%s ", operation);
    //printf("%s\n", storeStr);
    unsigned int accessAdder = 100;
    int accessSize;
    //unsigned int storeData;
    // char adderHexstring[40];
    char dataHexstring[65];
    char loadDataHexstring[65];

    fscanf(loadStoreFile, " 0x%x", &accessAdder);

    //printf("%s %x\n", "accessAdder:", accessAdder);
    fscanf(loadStoreFile, " %d\n", &accessSize);
    // printf("%s %d\n", "accessSize:", accessSize);

    if (compareToStore == 0){
      fscanf(loadStoreFile, " %s\n", &dataHexstring);
      //storeData = (unsigned int)strtol(dataHexstring, NULL, 16);
      //printf("%s\n", "from store");
      //printf("%s %s\n","storeData hexstring:", dataHexstring);
    }


    // unsigned int offsetBitmask = ((1<<numOffsetBits) - 1);
    // unsigned int offsetBits = offsetBitmask & accessAdder;
    // int tagBitmask = (1<<(accessAdder>>numOffsetBits + numOffsetBits)) - 1;
    // int tagBits = tagBitmask & accessAdder;
    int offsetBitmask = ((1<<numOffsetBits) - 1);
    int offsetBits = offsetBitmask & accessAdder;
    int tagBits = accessAdder>>(numSetBits + numOffsetBits);
    int setBitMask = ((1<<numSetBits) - 1);
    setBitMask = setBitMask<<numOffsetBits;
    int setBits = accessAdder & setBitMask;
    setBits = setBits >> numOffsetBits;

    int accessIndex = accessAdder; // first index to store/load
    // if (setBits == 2){
    //   DEBUG = 1;
    // }

    if (DEBUG == 1){
      printf("%s %d\n", "numOffsetBits:", numOffsetBits);
      printf("%s %d\n", "numSetBits:", numSetBits);
      printf("%s %d\n", "tagBits:", tagBits);
      printf("%s %d\n", "SetBits:", setBits);
      printf("%s %d\n", "offsetBits:", offsetBits);
    }

// FOR STORES
    if (compareToStore == 0){
      for (int i = 0; i < accessSize * 2; i+=2){ // storing data in main mem(s)
        char nextChar = dataHexstring[i];
        char nextNextChar = dataHexstring[i+1];
        mainMemory[accessIndex] = nextChar;
        mainMemoryTwo[accessIndex] = nextNextChar;
        // printf("%s %c\n", "This is what you stored:", nextChar);
        // printf("%s %c\n", "This is what you stored next:", nextNextChar);
        accessIndex += 1;
      }
    //  check each way for your tag - if not there it's a miss
    int storeHit = 0;
    for (int j = 0; j < associativity; j++){ // figure out if hit or miss based on if tag is there
      if (DEBUG == 1){
        printf("%s %d %s %d %s %d\n", "tag bits for way", j, "are:", myCache[setBits][j].tag, "and valid bits are:", myCache[setBits][j].valid);
    }
      if (myCache[setBits][j].tag == tagBits && myCache[setBits][j].valid == 1){
        storeHit = 1;
        myCache[setBits][j].recentNum = counter;
        break;
      }
    }
    printf("%s", "0x", accessAdder);
    printf("%x ", accessAdder);
    if (storeHit == 0){
      printf("%s\n", "miss");
    }
    else{
      printf("%s\n", "hit");

    }
    if (DEBUG == 1){
      printf("%s\n", "********************************");
    }
    }


// FOR LOADS
    else{
      for (int i = 0; i < accessSize * 2; i+=2){ // loading data from main mem(s)
        if (mainMemory[accessIndex] != NULL){
          loadDataHexstring[i] = mainMemory[accessIndex];
        }
        else{
          loadDataHexstring[i] = zeroChar;
        }
        if (mainMemoryTwo[accessIndex] != NULL){
        loadDataHexstring[i+1] = mainMemoryTwo[accessIndex];
        }
        else{
          loadDataHexstring[i+1] = zeroChar;
        }
        // printf("%s %c\n", "This is what you loaded:", mainMemory[accessIndex]);
        // printf("%s %c\n", "This is what you loaded next:", mainMemoryTwo[accessIndex]);
        accessIndex += 1;
      }
      loadDataHexstring[accessSize*2] = NULL;
      // printf("%s %s\n","loadData hexstring:", loadDataHexstring);

      int loadHit = 0;
      for (int j = 0; j < associativity; j++){
        if (DEBUG == 1){
        printf("%s %d %s %d %s %d\n", "tag bits for way", j, "are:", myCache[setBits][j].tag, "and valid bits are:", myCache[setBits][j].valid);
      }
        if (myCache[setBits][j].tag == tagBits && myCache[setBits][j].valid == 1){
          if (DEBUG == 1){
          printf("%s %d %s %d %s %d\n", "We have a hit! and tag bits for", j, "are:", myCache[setBits][j].tag, "and valid bits are:", myCache[setBits][j].valid);
        }
          loadHit = 1;
          myCache[setBits][j].recentNum = counter;
          break;
        }
        //printf("%s %d\n", "tag for way", myCache[setBits][j].tag);
      }
        if (loadHit == 0){
          int leastRecentWayIndex = -1;
          int leastRecentWayValue = 2147483647;
          for (int j = 0; j < associativity; j++){ // figure out which way is least recently used
            //printf("%s %d\n", "least recent way val", leastRecentWayValue);
            if (myCache[setBits][j].valid != 1){
              leastRecentWayIndex = j;
              break;
            }
            if (myCache[setBits][j].recentNum <= leastRecentWayValue){
              leastRecentWayValue = myCache[setBits][j].recentNum;
              leastRecentWayIndex = j;

            }
          }
          // printf("%s %d\n", "tag bits for way before load miss", myCache[setBits][leastRecentWayIndex].tag);
          // printf("%s %d\n", "valid bit before load miss", myCache[setBits][leastRecentWayIndex].valid);

          myCache[setBits][leastRecentWayIndex].recentNum = counter;
          myCache[setBits][leastRecentWayIndex].tag = tagBits;
          myCache[setBits][leastRecentWayIndex].valid = 1;
          //printf("%s %d\n", "tag bits for way after load miss", myCache[setBits][leastRecentWayIndex].tag);
          //printf("%s %d\n", "valid bit after load miss", myCache[setBits][leastRecentWayIndex].valid);
        }
        printf("%s", "0x", accessAdder);
        printf("%x ", accessAdder);
        if (loadHit == 0){
          printf("%s ", "miss");
        }
        else{
          printf("%s ", "hit");
        }
        printf("%s\n",loadDataHexstring);
        if (DEBUG == 1){
          printf("%s\n", "********************************");
        }
    }
    counter += 1;

  }
  return 0;
}

int main(int argc, char *argv[]){

  char* filename = argv[1];
  FILE* loadStoreFile = fopen(filename, "r"); /*this is a pointer to my cache file*/
  if (loadStoreFile == NULL) { /*in case invalid cache file*/
    printf("%s\n","PAGE FILE IS NULL");
    return 0;
  }
  int cacheSize = atoi(argv[2]); /*in KB, not including tag bits*/
  int associativity = atoi(argv[3]);
  int blockSize = atoi(argv[4]); /*in bytes*/

  cacheSim(loadStoreFile, cacheSize, associativity, blockSize);

  return 0;

}
