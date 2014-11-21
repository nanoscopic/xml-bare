#include"sh_page_manager.h"
#include"sh_hash.h"
#include"sh_bighash.h"
#include"sh_page.h"
#include"sh_shared.h"

#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int sh_page_manager__store_z( sh_page_manager *self, sh_hash *hashob, char *name, STORED_TYPE value ) {
  #ifdef DEBUG
  printf("\nPagenum:%i\n",hashob->mypage );
  #endif
  sh_page *pageob = sh_page_manager__getpage_by_pagenum( self, hashob->mypage );
  return sh_hash__store_z( hashob, self, pageob, name, value );
}

STORED_TYPE *sh_page_manager__del( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen ) {
  #ifdef DEBUG
  printf("\nPagenum:%i\n",hashob->mypage );
  #endif
  sh_page *pageob = sh_page_manager__getpage_by_pagenum( self, hashob->mypage );
  return sh_hash__del( hashob, self, pageob, name, namelen );
}

int sh_page_manager__store( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen, STORED_TYPE value ) {
  #ifdef DEBUG
  printf("\nPagenum:%i\n",hashob->mypage );
  #endif
  sh_page *pageob = sh_page_manager__getpage_by_pagenum( self, hashob->mypage );
  return sh_hash__store( hashob, self, pageob, name, namelen, value );
}

int sh_page_manager__store_u1( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen, u1 value ) {
    uint64_t bigval = value;
    void *ptr = ( void * ) bigval;
    return sh_page_manager__store( self, hashob, name, namelen, ptr );
}

int sh_page_manager__store_u2( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen, u2 value ) {
    uint64_t bigval = value;
    void *ptr = ( void * ) bigval;
    return sh_page_manager__store( self, hashob, name, namelen, ptr );
}

int sh_page_manager__store_u4( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen, u4 value ) {
    uint64_t bigval = value;
    void *ptr = ( void * ) bigval;
    return sh_page_manager__store( self, hashob, name, namelen, ptr );
}

int sh_page_manager__store_str( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen, char *str, uint16_t strlen ) {
    string_with_len *strnode = ( string_with_len * ) malloc( sizeof( string_with_len ) );
    strnode->str = str;
    strnode->len = strlen;
    return sh_page_manager__store( self, hashob, name, namelen, strnode );
}


int sh_page_manager__store_bighash( sh_page_manager *self, sh_bighash *hashob, char *name, u1 namelen, STORED_TYPE value ) {
  return sh_bighash__store( hashob, self, name, namelen, value );
}

STORED_TYPE *sh_page_manager__fetch_z( sh_page_manager *self, sh_hash *hashob, char *name ) {
  return sh_hash__fetch_z( hashob, self, sh_page_manager__getpage_by_pagenum( self, hashob->mypage ), name );
}

STORED_TYPE *sh_page_manager__fetch( sh_page_manager *self, sh_hash *hashob, char *name, u1 namelen ) {
  #ifdef DEBUG
  printf("Attempting to fetch name \"%.*s\" from hash %p\n", namelen, name, hashob );
  #endif
  sh_page *page = sh_page_manager__getpage_by_pagenum( self, hashob->mypage );
  return sh_hash__fetch( hashob, self, page, name, namelen );
}

STORED_TYPE *sh_page_manager__fetch_bighash( sh_page_manager *self, sh_bighash *hashob, char *name, u1 namelen ) {
  return sh_bighash__fetch( hashob, self, name, namelen );
}

sh_page_manager *sh_page_manager__new() {
  sh_page_manager *self = ( sh_page_manager *) malloc( sizeof( struct sh_page_manager__s ) );//new sh_page_manager_;
  if( !self ) {
      printf("Cannot allocate page manager structure");
      exit(1);
  }
  memset( self, 0, sizeof( struct sh_page_manager__s ) );
  self->curpage = 0;
  memset( self->pages, 0, MAX_NUM_PAGES*sizeof( sh_page * ) ); // Can have up to 255 pages
  self->pages[0] = sh_page__new( 0 ); // Create the very first page
  return self;
}

void sh_page_manager__destroy( struct sh_page_manager__s *self ) {
  sh_page__destroy( self->pages[0] ); // Wipe out the first page
  // TODO: Destroy any pages that were created
}

sh_page *sh_page_manager__getpage_by_pagenum( struct sh_page_manager__s *self, PAGE_ID_TYPE pagenum ) {
  #ifdef DEBUG
  printf("Attempting to fetch page number %i\n", pagenum );
  #endif
  sh_page *ref = self->pages[ pagenum ];
  if( !ref ) {
    ref = self->pages[ pagenum ] = sh_page__new( pagenum );
    if( !ref ) {
      printf("Cannot allocate\n");
      exit(1);
    }
    #ifdef DEBUG
    printf("Created new page %i - %p\n", pagenum, ref );
    #endif
  }
  #ifdef DEBUG
  printf("Got right page? %i == %i\n", pagenum, ref->id );
  #endif
  return ref;
}

sh_page *sh_page_manager__getpage( struct sh_page_manager__s *self ) {
  #ifdef DEBUG
  printf("Attempt to fetch curpage\n");
  printf("%i\n",self->curpage);
  #endif
  return sh_page_manager__getpage_by_pagenum( self, self->curpage );
}

sh_page *sh_page_manager__next_emptyish_page( struct sh_page_manager__s *self ) {
  PAGE_ID_TYPE i;
  for( i=0; i<=self->curpage; i++ ) {
    sh_page *ref = self->pages[ i ];
    if( sh_page__emptyish( ref ) ) return ref;
  }
  return sh_page_manager__getpage_by_pagenum( self, i );// automatically creates a new page
}

int page_full( sh_page *self ) {
  return ( self->stored >= 7000 );
}

sh_hash *sh_page_manager__new_hash( struct sh_page_manager__s *self ) {
  PAGE_ID_TYPE curpage = self->curpage;
  #ifdef DEBUG
  printf("Attempt to create hash - curpage = %i\n", curpage );
  #endif
  sh_page *pageob = sh_page_manager__getpage( self );
  if( sh_page__full( pageob ) ) {
    self->curpage++;
    pageob = sh_page_manager__getpage( self );
  }
  sh_hash *newhash = sh_hash__new( curpage, pageob );
  if( !newhash ) {
    printf("Cannot allocate\n");
    exit(1);
  }
  
  return newhash;
}

sh_hash *sh_page_manager__new_hash_fullpage( struct sh_page_manager__s *self ) {
  #ifdef DEBUG
  printf("Attempt to create hash\n");
  #endif
  self->curpage++;
  sh_page *pageob = sh_page_manager__getpage( self );
  sh_hash *newhash = sh_hash__new( self->curpage, pageob );
  if( !newhash ) {
    printf("Cannot allocate\n");
    exit(1);
  }
  return newhash;
}

sh_bighash *sh_page_manager__new_bighash( sh_page_manager *self, u4 desired_size ) { // create a new "big hash"
  sh_bighash *newhash = sh_bighash__new( self, desired_size );
  return newhash;
}

void sh_page_manager__debug( struct sh_page_manager__s *self ) {
  PAGE_ID_TYPE i;
  printf("Dumping debug information for all existing pages\n");
  u4 totstored = 0;
  for( i=0; i<=self->curpage; i++ ) {
    sh_page *ref = self->pages[ i ];
    sh_page__debug( ref );
    totstored += ref->stored;
  }
  printf("  Total stored items: %li\n", ( unsigned long int ) totstored );
}