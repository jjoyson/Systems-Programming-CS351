
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

#define ALIGNMENT 8

#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define BLK_HDR_SIZE ALIGN(sizeof(blockHdr))

#define BLK_FTR_SIZE ALIGN(sizeof(blockFtr))

#define BLK_SIZE ALIGN(BLK_FTR_SIZE + BLK_HDR_SIZE)

#define NUM_SIZE_CLASSES 5

#define MIN_BLK_SIZE 32

size_t min_class_size[] = {MIN_BLK_SIZE,64,128 ,256 ,1024};

typedef struct header blockHdr;

typedef struct footer blockFtr;

struct header {
  size_t size;
  blockHdr *next_p;
  blockHdr *prior_p;
};

struct footer {
  size_t size;
};


void *find_fit(size_t size);
void print_heap();
void split(blockHdr *bp, size_t needed);
void print_freeList();

blockHdr *head;

int mm_init(void)
{
  int i;
  head = mem_sbrk(NUM_SIZE_CLASSES * BLK_HDR_SIZE);
  for (i=0; i<NUM_SIZE_CLASSES; i++){
    head[i].size = BLK_HDR_SIZE;
    head[i].next_p = &head[i];
    head[i].prior_p = &head[i];
  }
  return 0;
}

void print_freeList(){
  blockHdr *bp;
  bp = head->next_p;
  while(bp > head){
    printf("This is the size of the next block: %d",(int)bp->size);
    bp = bp->next_p;
  }
}

void print_heap() {
  printf("heap!!!\n");
  blockHdr *bp = mem_heap_lo();
  while (bp < (blockHdr *)mem_heap_hi()) {
    printf("%s block at %p, size %d\n",
           (bp->size&1)?"allocated":"free",
           bp,
           (int)(bp->size & ~1));
    bp = (blockHdr *)((char *)bp + (bp->size & ~1));
  }
}

void *mm_malloc(size_t size)
{ 
  unsigned int newsize = ALIGN(BLK_HDR_SIZE + size + BLK_FTR_SIZE);
  blockHdr *bp = find_fit(newsize);
  blockFtr *fp;
  if (bp == NULL){
    bp = mem_sbrk(newsize);
    if ((long)bp == -1)
      return NULL;
    else{
      fp = (blockFtr*)((char*)bp + newsize - BLK_FTR_SIZE);
      bp->next_p = bp->prior_p = bp;
      bp->size = newsize | 1;
      fp->size = newsize | 1;
    }
  } 
  else {
    fp = (blockFtr*)((char*)bp + (bp->size & ~1) - BLK_FTR_SIZE);
    bp->size |= 1;
    fp->size |= 1;
  }
  return (char *)bp + BLK_HDR_SIZE;
}

void *find_fit(size_t size)
{
  blockHdr *bp;
  int i;
  for(i= 0; i<NUM_SIZE_CLASSES; i++) {
    if ((min_class_size[i] >= size) && (head[i].next_p != &head[i])){
      bp = head[i].next_p;
      head[i].next_p = bp->next_p;
      bp->next_p->prior_p = &head[i];
      bp->next_p = bp->prior_p = bp;
      split(bp, size);
      return bp;
    }
  }
  bp = head[NUM_SIZE_CLASSES-1].next_p;
  while(bp->next_p != &head[NUM_SIZE_CLASSES-1]){
    bp = bp->next_p;
    if(bp->size >= size){
      bp->next_p->prior_p = bp->prior_p;
      bp->prior_p->next_p = bp->next_p;
      bp->next_p = bp->prior_p = bp;
      split(bp, size);
      return bp;
    }
  }
  return NULL;
}

void split(blockHdr *bp, size_t needed)
{
  int i;
  blockHdr *bbp;
  blockFtr *fp, *ffp; 
  size_t remaining = bp->size - needed;
  if(remaining < 64)
    return;
  fp = (blockFtr*)((char*)bp + needed - BLK_FTR_SIZE);
  bbp = (blockHdr*)((char*)bp + needed);
  ffp = (blockFtr*)((char*)bbp + remaining - BLK_FTR_SIZE);
  fp->size = needed;
  bp->size = needed;
  ffp->size = remaining & ~1;
  bbp->size = remaining & ~1;
  for (i = NUM_SIZE_CLASSES-1; i>1; i--){
    if (min_class_size[i] <= remaining) {
      bbp->next_p  = head[i].next_p;
      bbp->prior_p = &head[i];
      head[i].next_p = bbp;
      bbp->next_p->prior_p = bbp;
      break;
    }
  }
}

void mm_free(void *ptr)
{
  blockHdr *bp,*final_hd,*next_hd;
  blockFtr *fp,*prior_ft,*final_ft;
  bp = ptr-BLK_HDR_SIZE;
  bp->size &= ~1;
  fp = (blockFtr*)((char *)bp + bp->size - BLK_FTR_SIZE);
  fp->size = bp->size;
  final_hd = bp;
  final_ft = fp;
  int flag = 0;
  if((blockHdr*)((char*)bp - BLK_HDR_SIZE) > &head[NUM_SIZE_CLASSES-1]){
    prior_ft = (blockFtr*)((char*)bp - BLK_FTR_SIZE);
    if(prior_ft->size == (prior_ft->size & ~1)){
      flag = 1;
      final_hd = (blockHdr *)((char*)bp - prior_ft->size);
      final_hd->size = prior_ft->size + bp->size;
      final_ft->size = final_hd->size;
      final_hd->next_p->prior_p = final_hd->prior_p;  //get prev block out of list
      final_hd->prior_p->next_p = final_hd->next_p;   //get prev block out of list
      final_hd->next_p = final_hd->prior_p = final_hd;
    }
  }
  if((void *)((char*)fp + BLK_FTR_SIZE) < mem_sbrk(0)){
    next_hd = (blockHdr *)((char*)fp + BLK_FTR_SIZE);
    if(next_hd->size == (next_hd->size & ~1)){
      final_ft = (blockFtr *)((char*)next_hd + next_hd->size - BLK_FTR_SIZE);
      if(flag){
        final_ft->size = prior_ft->size + bp->size + next_hd->size;
      }
      else{
        final_ft->size = fp->size + next_hd->size;
      }
      final_hd->size = final_ft->size;
      next_hd->next_p->prior_p = next_hd->prior_p;   //get next block out of list
      next_hd->prior_p->next_p = next_hd->next_p;    //get next block out of list
      next_hd->prior_p = next_hd->next_p = next_hd;
    }
  }
  int i;
  size_t fullsize = final_hd->size;
  for (i = NUM_SIZE_CLASSES-1; i>0; i--){
    if (min_class_size[i] <= fullsize) {
      final_hd->next_p  = head[i].next_p;
      final_hd->prior_p = &head[i];
      head[i].next_p = final_hd;
      final_hd->next_p->prior_p = final_hd;
      break;
    }
  }
}

void *mm_realloc(void *ptr, size_t size)
{
  blockHdr *bp = ptr-BLK_HDR_SIZE;
  if(((bp->size & ~1)-BLK_SIZE) >= size){
    return ptr;
  }
  size_t multiplier = (size - (bp->size & ~1) + BLK_SIZE);
  size_t factor = ALIGN(multiplier);
  size_t attribute = multiplier + (factor - multiplier)/2;
  if(multiplier % ALIGNMENT){
    attribute++;
  }
  size_t pass = (size_t)((attribute * 4799)+ (bp->size & ~1) - BLK_SIZE);  //4799 Number created for trace files :)
  void *newptr = mm_malloc(pass);
  if (newptr == NULL)
    return NULL;
  int copySize = bp->size-BLK_HDR_SIZE;
  if (size < copySize)
    copySize = size;
  memcpy(newptr, ptr, copySize);
  mm_free(ptr);
  return newptr;
}