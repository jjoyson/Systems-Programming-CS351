#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>

int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;

int flag = 0;
int s = 0;
int setBits = 0;
int associativity = 0;
int b = 0;
int blockBits = 0;
char *trace = 0;
unsigned long long lastUsedCounter = 0;

typedef struct cacheUselessName cache_t;

struct cacheUselessName{
	char valid; //1 bit
	unsigned long long tag;
	//unsigned long data;
	unsigned long long lBit;
};
cache_t **c;
//unsigned long long mask;

void printer (char *argv[]);
void make_Cache();
void free_Cache();
void use_trace(char *filename);
void cache_input(unsigned long long dataAddress);


int main(int argc, char *argv[])
{
	char c;
	while ((c = getopt(argc, argv, "s:E:b:t:hv")) != -1) {
    	switch (c) {
    		case 'h':
    			printer(argv);
    			break;
    		case 'v':
    			flag = 1;
    			break;
    		case 's':
    			s = atoi(optarg);
    			setBits = pow(2.0, s);
    			break;
    		case 'E':
    			associativity = atoi(optarg);
    			break;
    		case 'b':
    			b = atoi(optarg);
    			blockBits = pow(2.0, b);
    			break;
   			case 't':
    			trace = optarg;
    			break;
   			default:
   			 	printer(argv);
    			exit(0);
   		 }
	}

	if(setBits <= 0 || associativity <= 0 || blockBits <= 0){
		printf("%s: Missing required command line argument\n", argv[0]);
		printer(argv);
		exit(0);
	}
	// Once all the checks are done
	make_Cache();	
	use_trace(trace);
	free_Cache();
	printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

//void printSummary(int hit_count, miss_count, eviction_count);

void printer (char *argv[]){
	//don't need this but mimixing the ref
	printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
 	printf("Options:\n");
  	printf("  -h         Print this help message.\n");
  	printf("  -v         Optional verbose flag.\n");
  	printf("  -s <num>   Number of set index bits.\n");
  	printf("  -E <num>   Number of lines per set.\n");
  	printf("  -b <num>   Number of block offset bits.\n");
  	printf("  -t <file>  Trace file.\n");
  	printf("\n");
  	printf("Examples:\n");
  	printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
  	printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
  	exit(0);
}

void make_Cache(){
	c = (cache_t**)malloc(setBits*sizeof(cache_t*));			// Save data for sets
	for(int i = 0; i < setBits; i++){
		c[i] = (cache_t*)malloc(associativity*sizeof(cache_t));	// lines per set 
		for(int j = 0; j < associativity; j++){
			c[i][j].valid = 0;									// data in each line
			c[i][j].tag = 0;
			c[i][j].lBit = 0;
		}
	}
}
	
void free_Cache(){
	for(int i = 0; i < setBits; i++){
		free(c[i]);												// Second malloc
	}
	free(c);													// First malloc
}

void use_trace(char *filename){
	FILE *infile;
 	char buff;
 	unsigned long long address;
 	int length;

  	if ((infile = fopen(filename, "r")) == NULL) {
   		printf("Error opening tracefile %s\n", filename);
    	exit(1);
 	}

 	while (fscanf(infile, " %c %llx,%d", &buff, &address, &length) != EOF) {
 		if(buff == 'S' || buff == 'L' || buff == 'M'){
 			if(flag)
 				printf("%c  %llx,%d ", buff, address, length);
 			cache_input(address);
 			if(buff == 'M')
 				cache_input(address);
 			if(flag)
 				printf("\n");
 		}
 	}
 	fclose(infile);
 }

void cache_input(unsigned long long dataAddress){
	unsigned long long tag = dataAddress >> (s + b);
	unsigned long long setMask = (setBits - 1);
	unsigned long long setIndex = (dataAddress >> b) & setMask;

	for(int i =0; i < associativity; i++){
		if((c[setIndex][i].tag == tag) && (c[setIndex][i].valid)){    //Same tag and is valid
			c[setIndex][i].lBit = lastUsedCounter;
			lastUsedCounter++;
			hit_count++;
			if(flag)
				printf("hit ");
			return;
		}
	}

	miss_count++;
	if(flag)
		printf("miss ");

	int j = 0;
	unsigned long long check = c[setIndex][0].lBit;
	for(int i = 0; i < associativity; i++){
		if (c[setIndex][i].lBit < check){
			check = c[setIndex][i].lBit;
			j = i;
		}
	}
	
	if(c[setIndex][j].valid){
		eviction_count++;
		if(flag)
			printf("eviction ");		
	}

	c[setIndex][j].tag = tag;
	c[setIndex][j].valid = 1;
	c[setIndex][j].lBit = lastUsedCounter;
	lastUsedCounter++;
}