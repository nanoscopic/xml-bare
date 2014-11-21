#ifndef SH_HASH_H
#define SH_HASH_H

#include<stdint.h>
#include"sh_shared.h"

struct sh_hash__s {
  HASH_ID_TYPE id; // id used within the page
  u4 offset;
  sh_hash *next;
  PAGE_ID_TYPE mypage; // page used by this hash
  struct storedstr *head;
  u2 numstored; // this value is not really needed...
};

sh_hash     *sh_hash__new( PAGE_ID_TYPE pagenum, sh_page *pageob );
sh_hash     *sh_hash__new_setdetails( PAGE_ID_TYPE pagenum, sh_page *pageob, HASH_ID_TYPE id, u4 offset );
int          sh_hash__store_z( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, STORED_TYPE value );
STORED_TYPE *sh_hash__del(     sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, u1 namelen );
int          sh_hash__store(   sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, u1 namelen, STORED_TYPE value );
STORED_TYPE *sh_hash__fetch_z( sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name );
STORED_TYPE *sh_hash__fetch(   sh_hash *self, sh_page_manager *man, sh_page *pageob, char *name, u1 namelen );
void         sh_hash__debug(   sh_hash *self );

#endif