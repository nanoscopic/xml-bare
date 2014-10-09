#include"sh_shared.h"
#include"sh_bighash.h"
#include"sh_page_manager.h"
#include"sh_hash_func.h"
#include"sh_hash.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

sh_bighash *sh_bighash__new( sh_page_manager *man, u4 desired_size ) {
  sh_bighash *self = (sh_bighash *) malloc( sizeof( sh_bighash ) );//new sh_bighash;
  memset( self, 0, sizeof( sh_bighash ) );
  u2 needed_pages = ( desired_size / PAGE_DIVIDER ) + 1;
  self->num_hashes = needed_pages;
  #ifdef DEBUG
  printf("\nCreating sh_bighash; number of pages/hashes:%i\n",self->num_hashes );
  #endif
  u2 i;
  for( i=0;i<self->num_hashes;i++ ) {
    self->hashes[i] = sh_page_manager__new_hash_fullpage( man );
  }
  self->numstored = 0;
  return self;
}

int sh_bighash__store_z( sh_bighash *self, sh_page_manager *man, char *name, STORED_TYPE value ) {
  return sh_bighash__store( self, man, name, strlen( name ), value );
}

int sh_bighash__store( sh_bighash *self, sh_page_manager *man, char *name, u1 namelen, STORED_TYPE value ) {
  u4 part_num = hashstr_mul( name, namelen, 241, 0 );
  u2 hash_i = part_num % self->num_hashes;
  #ifdef DEBUG
  printf("\nFetching hash %i within sh_bighash\n", hash_i );
  #endif
  sh_hash *hash = self->hashes[ hash_i ];
  int res = sh_page_manager__store( man, hash, name, namelen, value );
  if( res == -1 ) return -1;
  self->numstored++;
  return res;
}

STORED_TYPE *sh_bighash__fetch_z( sh_bighash *self, sh_page_manager *man, char *name ) {
  return sh_bighash__fetch( self, man, name, strlen( name ) );
}

STORED_TYPE *sh_bighash__fetch( sh_bighash *self, sh_page_manager *man, char *name, u1 namelen ) {
  u4 part_num = hashstr_mul( name, namelen, 241, 0 );
  u2 hash_i = part_num % self->num_hashes;
  #ifdef DEBUG
  printf("\nFetching hash %i within sh_bighash\n", hash_i );
  #endif
  sh_hash *ahash = self->hashes[ hash_i ];
  return sh_page_manager__fetch( man, ahash, name, namelen );
  //return hash->fetch( man, man->getpage( hash->mypage ), name, namelen );
}

void sh_bighash__debug( sh_bighash *self ) {
  printf("Printing debug information for sh_bighash\n");
  u2 i;
  for( i=0;i<self->num_hashes;i++ ) {
    sh_hash *ahash = self->hashes[ i ];
    sh_hash__debug( ahash );
  }
}