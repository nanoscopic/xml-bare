#ifndef SH_BIGHASH_H
#define SH_BIGHASH_H

#include<stdint.h>
#include"sh_shared.h"

struct sh_bighash__s {
  u2 num_hashes;
  sh_hash *hashes[1000];
  u4 numstored;
};

sh_bighash  *sh_bighash__new(                       sh_page_manager *man, u4 desired_size );
int          sh_bighash__store_z( sh_bighash *self, sh_page_manager *man, char *name, STORED_TYPE value );
int          sh_bighash__store(   sh_bighash *self, sh_page_manager *man, char *name, u1 namelen, STORED_TYPE value );
STORED_TYPE *sh_bighash__fetch_z( sh_bighash *self, sh_page_manager *man, char *name );
STORED_TYPE *sh_bighash__fetch(   sh_bighash *self, sh_page_manager *man, char *name, u1 namelen );
void         sh_bighash__debug(   sh_bighash *self );

#endif