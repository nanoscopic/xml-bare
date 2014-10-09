#ifndef SH_PAGE_H
#define SH_PAGE_H

#include"sh_shared.h"

struct sh_page__s {
  u4 offset;
  HASH_ID_TYPE curid;
  struct entry set[ PAGE_SIZE ];
  u2 stored;
  PAGE_ID_TYPE id;
};

sh_page     *sh_page__new( PAGE_ID_TYPE id );
int          sh_page__store_z(   sh_page *self, HASH_ID_TYPE id, char *name, u4 offset, STORED_TYPE value ); // null terminated string
int          sh_page__store(     sh_page *self, HASH_ID_TYPE id, char *name, u1 namelen, u4 offset, STORED_TYPE value );
STORED_TYPE *sh_page__fetch_z(   sh_page *self, HASH_ID_TYPE id, char *name, u4 offset  );
STORED_TYPE *sh_page__fetch(     sh_page *self, HASH_ID_TYPE id, char *name, u1 namelen, u4 offset  ); // null terminated string
HASH_ID_TYPE sh_page__newid(     sh_page *self );
u4           sh_page__newoffset( sh_page *self );
u1           sh_page__emptyish(  sh_page *self );
u1           sh_page__full(      sh_page *self );
void         sh_page__debug(     sh_page *self );
void         sh_page__destroy(   sh_page *self );

#endif