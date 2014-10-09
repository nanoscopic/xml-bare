#ifndef SH_SHARED_H
#define SH_SHARED_H

#include<stdint.h>

#define USE_VERIFY
#define VERIFY_DIVIDER 65521 // closest prime <= 65,536
#define PAGE_SIZE      10000
#define PAGE_CHUNKS       20 //
#define PAGE_DIVIDER    9973 // closest prime <= 10,000
#define HASH_ID_TYPE uint16_t
#define PAGE_FULL_MULT   0.8

#define PAGE_ID_TYPE uint16_t
#define MAX_NUM_PAGES 556
#define STORED_TYPE void *

typedef uint64_t u8;
typedef uint32_t u4;
typedef uint16_t u2;
typedef uint8_t  u1;

/*
The "multiple try" feature enables attempting to rehash the key after offsetting it
some up to N times if there is a collision. This is done within a single page.
There is already support in the hash class to store to a new subhash ( on it's
own page ) when there is a collision.

In theory enabling "multiple trys" could be faster if the size of the page fits
in cache.
*/
#define MULTI_TRY 10
#define INC 6793

//#define DEBUG

long gettot();
void setinc( int newinc );
void cleartot();

struct entry {
  HASH_ID_TYPE owner;//2
  STORED_TYPE index;//4 ( 32-bit )
  #ifdef USE_VERIFY
  u2 verify;//2
  #endif
  u1 namelen;//1
  char *name; // TODO: use a string table // 2
};

struct string_with_len_s {
    char *str;
    u2 len;
};
typedef struct string_with_len_s string_with_len;

struct storedstr {
  struct storedstr *next;
  char *str;
};

// forward declaration of various structed that exist
struct sh_pageman__s;
struct sh_page__s;
struct sh_hash__s;
struct sh_bighash__s;

//typedef struct sh_pageman__s sh_page_manager__;
typedef struct sh_page__s sh_page;
typedef struct sh_hash__s sh_hash;
typedef struct sh_bighash__s sh_bighash;
typedef struct sh_page_manager__s sh_page_manager;

#endif
