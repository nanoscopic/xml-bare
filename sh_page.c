#include"sh_shared.h"
#include"sh_page.h"
#include<string.h>
#include<stdio.h>
#include"sh_hash_func.h"
#include<stdlib.h>

HASH_ID_TYPE sh_page__newid( sh_page *self ) {
    return self->curid++;
}

void sh_page__destroy( sh_page *self ) {
    free( self );
}

sh_page *sh_page__new( PAGE_ID_TYPE id ) {
  sh_page *self = (sh_page *) malloc( sizeof( sh_page ) );//new page;
  memset( self, 0, sizeof( sh_page ) );
  self->stored = 0;
  self->offset = 0;
  self->id = id;
  self->curid = 1;
  memset( self->set, 0, PAGE_SIZE*sizeof( struct entry ) );
  return self;
}

int primes[10] = { 0, 0, 104717, 104723, 104729, 92707 };

int sh_page__store_z( sh_page *self, HASH_ID_TYPE id, char *name, u4 offset, STORED_TYPE value ) {
  u1 namelen = strlen( name );
  return sh_page__store( self, id, name, namelen, offset, value );
}

int sh_page__store( sh_page *self, HASH_ID_TYPE hash_id, char *name, u1 namelen, u4 offset, STORED_TYPE value ) {
  #ifdef DEBUG
  printf("page::store hashid=%i offset=%lu page %p\n", self->id, ( unsigned long int ) self->offset, self );
  #endif
  
  u4 hval = hashstr( name, namelen, offset );
  
  #ifdef USE_VERIFY
  u2 verify = hval % VERIFY_DIVIDER; // prime
  #endif
  
  int setid = hval % PAGE_DIVIDER; // prime
  #ifdef DEBUG
  printf("about to store at index %i\n", setid );
  #endif
  
  #ifdef MULTI_TRY
  u1 trys = 1;
  while( trys <= MULTI_TRY ) {
  #endif
  
    struct entry *eptr = &self->set[ setid ];
    if( !eptr->owner ) {
      eptr->owner = hash_id;
      eptr->index = value;
      eptr->name  = name;
      eptr->namelen = namelen;
      #ifdef USE_VERIFY
      eptr->verify = verify;
      #endif
      
      #ifdef DEBUG
      printf("stored\n" );
      #endif
      
      self->stored++;
      return 0; // success { //&& ( eptr->owner != id ) ) {
    }
    else if( 
      eptr->owner == hash_id
      #ifdef USE_VERIFY
      && eptr->verify == verify
      #endif
      && namelen == eptr->namelen
      && !memcmp( name, eptr->name, namelen )
    ) { // pre-existing item is here
      printf("Conflict\n");
      eptr->index = value; // change the existing value
      return 0;
    }
  
  #ifdef MULTI_TRY
    trys++;
    //tottry++;
    offset += INC;
    hval = hashstr( name, namelen, offset );
    setid = hval % PAGE_DIVIDER;
    #ifdef USE_VERIFY
    verify = hval % VERIFY_DIVIDER; // prime
    #endif
  }
  #endif
  
  return -1;
}

u4 sh_page__newoffset( sh_page *self ) {//3,4=9007,6=7062,8=9545,11=8922
  // smaller than 16,711,661
  
  self->offset += 46817;//53467335;//148141639;
  //self->offset = self->offset % 16744379;
  return self->offset;
}

u1 sh_page__emptyish( sh_page *self ) {
  if( self->stored <= ( PAGE_FULL_MULT * PAGE_SIZE ) ) return 1;
  return 0;
}

u1 sh_page__full( sh_page *self ) {
  return ( self->stored >= ( PAGE_FULL_MULT * PAGE_SIZE ) );
}

STORED_TYPE *sh_page__fetch_z( sh_page *self, HASH_ID_TYPE id, char *name, u4 offset ) {
  // TODO calculate the length
  u1 namelen = strlen( name );
  return sh_page__fetch( self, id, name, namelen, offset );
}

STORED_TYPE *sh_page__fetch( sh_page *self, HASH_ID_TYPE hash_id, char *name, u1 namelen, u4 offset ) {
  u4 hval = hashstr( name, namelen, offset );
  int setid = hval % PAGE_DIVIDER;
  
  #ifdef USE_VERIFY
  u2 verify = hval % VERIFY_DIVIDER;
  #endif
  
  #ifdef DEBUG
  printf("Fetching at pos %i with hash id %i\n", setid, hash_id );
  #endif
  
  #ifdef MULTI_TRY
  u1 trys = 1;
  while( trys <= MULTI_TRY ) {
  #endif
  
    struct entry *eptr = &self->set[ setid ];
    if( 
      eptr->owner == hash_id
      #ifdef USE_VERIFY
      && eptr->verify == verify
      #endif
      && namelen == eptr->namelen
      && !memcmp( name, eptr->name, namelen )
    ) { // success
      return &eptr->index;
    }
    
  #ifdef MULTI_TRY
    trys++;
    offset += INC;
    hval = hashstr( name, namelen, offset );
    setid = hval % PAGE_DIVIDER;
    #ifdef USE_VERIFY
    verify = hval % VERIFY_DIVIDER;
    #endif
  }
  #endif
  
  #ifdef DEBUG
  printf("Failed attempting to fetch item %i\n",setid);
  #endif
  return NULL;
  
}

void sh_page__debug( sh_page *self ) {
  printf("  Page %i\n", self->id );
  printf("    Stored: %i\n", self->stored );
}