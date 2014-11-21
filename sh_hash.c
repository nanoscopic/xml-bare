#include"sh_shared.h"
#include"sh_hash.h"
#include<stdint.h>
#include"sh_page.h"
#include"sh_page_manager.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

sh_hash *sh_hash__new_setdetails( PAGE_ID_TYPE pagenum, sh_page *pageob, HASH_ID_TYPE id, uint32_t offset ) {
  #ifdef DEBUG
  printf("init hash\n");
  #endif
  sh_hash *self = (sh_hash *) malloc( sizeof( sh_hash ) );//new hash;
  memset( self, 0, sizeof( sh_hash ) );
  self->next = 0;
  self->mypage = pagenum;
  self->id = id;
  self->offset = offset;
  self->head = 0;
  self->numstored = 0;
  return self;
}

sh_hash *sh_hash__new( PAGE_ID_TYPE pagenum, sh_page *pageob ) {
  #ifdef DEBUG
  printf("init hash\n");
  #endif
  sh_hash *self = (sh_hash *) malloc( sizeof( sh_hash ) );//new hash;
  memset( self, 0, sizeof( sh_hash ) );
  self->next = 0;
  self->mypage = pagenum;
  self->id = sh_page__newid( pageob );
  self->offset = sh_page__newoffset( pageob );
  self->head = 0;
  self->numstored = 0;
  
  #ifdef DEBUG
  printf("Created hash %p with page %i\n", self, pagenum );
  #endif
  
  return self;
}

int sh_hash__store_z( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, STORED_TYPE value ) {
  uint8_t namelen = strlen( name );
  return sh_hash__store( self, man, pageob, name, namelen, value );
}

STORED_TYPE *sh_hash__del( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, uint8_t namelen ) {
  // attempt initial fetch
  if( pageob->id != self->mypage ) {
    printf("Wrong page passed in\n");
  }
  
  STORED_TYPE *res = sh_page__del( pageob, self->id, name, namelen, self->offset );
  if( res != NULL ) { return res; }
  //printf("fetch try 1 failed\n");
  
  if( self->next ) {
    sh_page *nextpage = sh_page_manager__getpage_by_pagenum( man, self->next->mypage );
    return sh_hash__del( self->next, man, nextpage, name, namelen );
  }
  
  return NULL;
}

int sh_hash__store( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, uint8_t namelen, STORED_TYPE value ) {
  #ifdef DEBUG
  printf("hash::store page=%i hashid=%i\n", pageob->id, self->id );
  #endif
  if( pageob->id != self->mypage ) {
    printf("Wrong page passed in\n");
  }
  
  // first try
  int res = sh_page__store( pageob, self->id, name, namelen, self->offset, value );
  if( res != -1 ) {
    self->numstored++;
    return res;
  }
  #ifdef DEBUG
  printf("try 1 failed" );
  #endif
  
  int trys = 1;
  
  trys++;
  
  // hand off to the next 'sub' hash if there is one
  if( self->next ) {
    sh_page *nextpage = sh_page_manager__getpage_by_pagenum( man, self->next->mypage );
    return sh_hash__store( self->next, man, nextpage, name, namelen, value );
  }
  
  /* Create a 'next' hash; preferring to avoid any collisions if possible
  
  This is not done using 'newhash' because we attempt to store under the specified
  id and offset before even creating the new hash object.
  
  TODO: Extend the logic so that new pages can be created as well in this process
  if the existing page is "full". There is a difference between going to the next
  page on a collision and abandoning a page because it is completely full.
  
  We don't want to abandon a page entirely on the first collision, because some
  number of collisions are acceptable before giving up on a page.
  */
  
  sh_page *nextpage = sh_page_manager__next_emptyish_page( man );
  PAGE_ID_TYPE npid = nextpage->id;
  uint32_t next_offset = sh_page__newoffset( nextpage );
  uint16_t hash_id = sh_page__newid( nextpage );//the hash id to assign within the next page
  
  while( trys <= 200 ) { // find the offset that works
    res = sh_page__store( nextpage, hash_id, name, namelen, next_offset, value );
    if( res != -1 ) {
      // success; create new next object ( using our same id )
      //printf("Created next hash with hash_id=%i and page_id=%i\n", hash_id, npid );
      self->next = sh_hash__new_setdetails( npid, nextpage, hash_id, next_offset );
      if( !self->next ) {
        printf("Cannot allocate\n");
        exit(1);
      }
      self->next->numstored++;
      return res;
    }
    next_offset = sh_page__newoffset( nextpage );
    trys++;
  }
  
  return -1;
}

STORED_TYPE *sh_hash__fetch_z( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name ) {
  // attempt initial fetch
  if( pageob->id != self->mypage ) {
    printf("Wrong page passed in\n");
  }
  
  STORED_TYPE *res = sh_page__fetch_z( pageob, self->id, name, self->offset );
  if( res != NULL ) { return res; }
  
  if( self->next ) {
    sh_page *nextpage = sh_page_manager__getpage_by_pagenum( man, self->next->mypage );
    printf("Attempting fetch from hash_id=%i and page_id=%i\n", self->next->id, nextpage->id );
    return sh_hash__fetch_z( self->next, man, nextpage, name );
  }
  
  return NULL;
}

STORED_TYPE *sh_hash__fetch( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, uint8_t namelen ) {
  // attempt initial fetch
  if( pageob->id != self->mypage ) {
    printf("Wrong page passed in\n");
  }
  
  STORED_TYPE *res = sh_page__fetch( pageob, self->id, name, namelen, self->offset );
  if( res != NULL ) { return res; }
  //printf("fetch try 1 failed\n");
  
  if( self->next ) {
    sh_page *nextpage = sh_page_manager__getpage_by_pagenum( man, self->next->mypage );
    return sh_hash__fetch( self->next, man, nextpage, name, namelen );
  }
  
  return NULL;
}

void sh_hash__debug( sh_hash *self ) {
  printf( "Printing debug information for hash %i in page %i\n", self->id, self->mypage );
  int len = 1;
  sh_hash *cur = self;
  printf("  (\n");
  while( 1 ) {
    cur = cur->next;
    if( !cur ) break;
    printf("    ---\n    Num stored:%i\n", cur->numstored );
    printf("    Id %i in page %i\n", cur->id, cur->mypage );
    len++;
  }
  printf("  )\n");
  printf("  Next length: %i\n", len );
}