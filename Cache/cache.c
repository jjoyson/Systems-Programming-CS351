/* 
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  trans.c in terms of its data cache performance.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */
if(M == 64){
int v1, v2, v3, v4, v5, v6, v7, v8; //Ideally, these go into the registers
int tmp;

  for (i = 0; i < 64; i+=8)
  {
    for (j = 0; j < 64; j += 8)
    {
      //8-by-8 block on the diagonal. Then split into 4, 4-by-4
      //blocks
      if (i == j) {
        for (v1 = 0; v1 < 4; v1++)
        {
          for (v2 = 0; v2 < 4; v2++)
          {
            if (v1 == v2) continue;
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }

          tmp = A[i+v1][i+v1];
          B[i+v1][i+v1] = tmp;
        }

        for (v1 = 4; v1 < 8; v1++)
        {
          for (v2 = 0; v2 < 4; v2++)
          {
            if ((v1-4) == v2) continue;
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }

          tmp = A[i+v1][i+(v1-4)];
          B[i+(v1-4)][i+v1] = tmp;
        }

        for (v1 = 4; v1 < 8; v1++)
        {
          for (v2 = 4; v2 < 8; v2++)
          {
            if (v1 == v2) continue;
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }

          tmp = A[i+v1][i+v1];
          B[i+v1][i+v1] = tmp;
        }

        for (v1 = 0; v1 < 4; v1++)
        {
          for (v2 = 4; v2 < 8; v2++)
          {
            if ((v1+4) == v2) continue;
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }

          tmp = A[i+v1][i+v1+4];
          B[i+v1+4][i+v1] = tmp;
        }
      }

      //8-by-8 block not on diagonal
      //See pdf for info
      else {
        for (v1 = 0; v1 < 4; v1++) { //Picture 1
          for (v2 = 0; v2 < 4; v2++) {
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }
        }

        //Picture 2
        v1 = A[i][j+4];
        v2 = A[i][j+5];
        v3 = A[i][j+6];
        v4 = A[i][j+7];
        v5 = A[i+1][j+4];
        v6 = A[i+1][j+5];
        v7 = A[i+1][j+6];
        v8 = A[i+1][j+7];

        for (k = 4; k < 8; k++) { //Picture 3
          tmp = A[i+2][j+k];
          B[j+2][i+k] = tmp;
          tmp = A[i+3][j+k];
          B[j+3][i+k] = tmp;
        }

        for (k = 4; k < 8; k++) { //Picture 4
            tmp = A[i+k][j+0];
            B[j+0][i+k] = tmp;
            tmp = A[i+k][j+1];
            B[j+1][i+k] = tmp;
        }

        for (k = 2; k < 4; k++) { //Picture 5
            tmp = B[j+k][i+4];
            B[j+4][i+k] = tmp;
            tmp = B[j+k][i+5];
            B[j+5][i+k] = tmp;
        }

        for (k = 4; k < 6; k++) { //Picture 6
            tmp = A[i+k][j+2];
            B[j+2][i+k] = tmp;
            tmp = A[i+k][j+3];
            B[j+3][i+k] = tmp;
        }

        //Picture 7
        B[j+4][i] = v1;
        B[j+4][i+1] = v5;
        B[j+5][i] = v2;
        B[j+5][i+1] = v6;

        //Picture 8
        v1 = B[j+2][i+6];
        v2 = B[j+2][i+7];
        v5 = B[j+3][i+6];
        v6 = B[j+3][i+7];

        for (k = 6; k < 8; k++) {
            tmp = A[i+k][j+2];
            B[j+2][i+k] = tmp;
            tmp = A[i+k][j+3];
            B[j+3][i+k] = tmp;
        }

        //Picture 10

        B[j+6][i] = v3;
        B[j+6][i+1] = v7;
        B[j+6][i+2] = v1;
        B[j+6][i+3] = v5;
        B[j+7][i] = v4;
        B[j+7][i+1] = v8;
        B[j+7][i+2] = v2;
        B[j+7][i+3] = v6;

        //Picture 11

        for (v1 = 4; v1 < 8; v1++) {
          for (v2 = 4; v2 < 8; v2++) {
            tmp = A[i+v1][j+v2];
            B[j+v2][i+v1] = tmp;
          }
        }
      }
    }
  }
}
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cachelab.h"
//#define DEBUG_ON 
#define ADDRESS_LENGTH 64
/* Type: Memory address */
typedef unsigned long long int mem_addr_t;
/* Type: Cache line
   LRU is a counter used to implement LRU replacement policy  */
typedef struct cache_line
{
  char valid;
  mem_addr_t tag;
  unsigned long long int lru;
} cache_line_t;
typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;
/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;
/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */
/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
unsigned long long int lru_counter = 1;
/* The cache we are simulating */
cache_t cache;
mem_addr_t set_index_mask;
/* 
 * initCache - Allocate memory, write 0's for valid and tag and LRU
 * also computes the set_index_mask
 */
void initCache()
{
  int i,j;
  cache = (cache_t) malloc(S*sizeof(cache_set_t));
  for(i=0;i<S;i++){
  
    cache[i] = (cache_set_t) malloc(E*sizeof(cache_line_t));
    for(j=0;j<E;j++) {
      cache[i][j].valid = 0;
      cache[i][j].tag = 0;
      cache[i][j].lru = 0;
    }
  }
  set_index_mask = (mem_addr_t) S-1;
}
/* 
 * freeCache - free allocated memory
 */
void freeCache()
{
  int i;
  for (i=0; i<S; i++)
    {
      free(cache[i]);
    }
  free(cache);
}
/* 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increast hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 */
void accessData(mem_addr_t addr)
{
  int i;
  mem_addr_t tag = addr >> (s+b);
  mem_addr_t set_index = (addr >> b) & set_index_mask;
  for(i = 0; i<E; i++) {
    if(cache[set_index][i].tag == tag && cache[set_index][i].valid == 1) {
      cache[set_index][i].lru = lru_counter++;
      if(verbosity == 1) printf("hit ");
      hit_count++;
      return;
    }
  }
  if(verbosity == 1) printf("miss ");
  miss_count++;


  unsigned long long int min[2];
  min[0] = ULONG_MAX; // Cache line
  min[1] = ULONG_MAX; // LRU
  for(i = 0; i<E; i++) {
    if(cache[set_index][i].lru < min[1]) {
      min[0] = (unsigned long long int) i;
      min[1] = cache[set_index][i].lru;
    }
  }
  i = (int) min[0];
  if(cache[set_index][i].valid == 1) {
    if(verbosity == 1) printf("eviction ");
    eviction_count++;
  }
  cache[set_index][i].valid = 1;
  cache[set_index][i].tag = tag;
  cache[set_index][i].lru = lru_counter++;
}
/*
 * replayTrace - replays the given trace file against the cache 
 */
void replayTrace(char* trace_fn)
{
  char buf[1000];
  char mode;
  mem_addr_t addr=0;
  unsigned int len=0;
  FILE* trace_fp = fopen(trace_fn, "r");
  if(!trace_fp){
    fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
    exit(1);
  }
  while(fgets(buf, 1000, trace_fp) != NULL)
    {
      sscanf(buf, " %c %llx,%u", &mode, &addr, &len);
      if(verbosity == 1) printf("%c %llx,%u ", mode, addr, len);
      if(mode == 'L'|| mode == 'S' || mode == 'M') accessData(addr);
      if(mode == 'M') accessData(addr);
      if(verbosity == 1) printf("\n");
    }
  fclose(trace_fp);
}
/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("\nExamples:\n");
  printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
  exit(0);
}
/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
  char c;
  while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1)
    {
      switch(c)
  {
  case 's':
    s = atoi(optarg);
    break;
        case 'E':
    E = atoi(optarg);
    break;
        case 'b':
    b = atoi(optarg);
    break;
        case 't':
    trace_file = optarg;
    break;
        case 'v':
    verbosity = 1;
    break;
        case 'h':
    printUsage(argv);
    exit(0);
        default:
    printUsage(argv);
    exit(1);
        }
    }
  /* Make sure that all required command line args were specified */
  if (s == 0 || E == 0 || b == 0 || trace_file == NULL)
    {
      printf("%s: Missing required command line argument\n", argv[0]);
      printUsage(argv);
      exit(1);
    }
  /* Compute S, E and B from command line args */
  S= 2 << (s-1);
  B= 2 << (b-1);
  /* Initialize cache */
  initCache();
#ifdef DEBUG_ON
  printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
  printf("DEBUG: set_index_mask: %llu\n", set_index_mask);
#endif
 
  replayTrace(trace_file);
  /* Free allocated memory */
  freeCache();
  /* Output the hit and miss statistics for the autograder */
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}
© 2016 Gogs Versie: 0.8.25.0129 Pagina: 204ms Sjabloon: 21ms  Nederlands  Website Go1.5.1