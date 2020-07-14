#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  /* FIXME: the current implementation doesn't update existing entries */
  unsigned int idx = hash(key) % ht->size;
  bucket_t *test = ht->buckets[idx];
  while(test){
    if((strcmp(test->key,key))== 0){
      free(test->val);
      test->val = val;
      free(key);
      return;
    }
    test = test->next;
  }
  bucket_t *b = malloc(sizeof(bucket_t));
  b->key = key;
  b->val = val;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;
}
 

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  int i = 0;
  bucket_t *b;
  while(i < ht->size){
    b = ht->buckets[i];
    bucket_t *c;
    while(b){
      c = b;
      b = b->next;
      free(c->val);
      free(c->key);
      free(c);
    }
    i++;
  }
  free(ht->buckets);
  free(ht); // FIXME: must free all substructures!
}

/* TODO */
void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  bucket_t *c;
  while(b){
    c = b;
    b = b->next;
    if((strcmp(c->key,key))== 0){
      ht->buckets[idx] = b;
      free(c->key);
      free(c->val);
      free(c);
      return;
    }
    if((strcmp(b->key,key))== 0){
      c->next = b->next;
      free(b->key);
      free(b->val);
      free(b);
      return;
    }
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  hashtable_t *bro = make_hashtable(newsize);
  int i = 0;
  while(i < ht->size){
    bucket_t *b = ht->buckets[i];
    bucket_t *c;
    while(b){
      c = b;
      b = b->next;
      ht_put(bro,c->key,c->val);
      free(c);
    }
    i++;
  }
  free(ht->buckets);
  ht->buckets = bro->buckets;
  ht->size = bro->size;
  free(bro);
}
