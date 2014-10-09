#ifndef SH_PAGEMAN_H
#define SH_PAGEMAN_H

#include<stdint.h>
#include"sh_shared.h"

struct sh_page_manager__s {
  sh_page *pages[ MAX_NUM_PAGES ];
  PAGE_ID_TYPE curpage;
};

sh_page_manager *sh_page_manager__new();
void            sh_page_manager__destroy(            sh_page_manager *self );
sh_hash        *sh_page_manager__new_hash(           sh_page_manager *self ); // create a new hash
sh_hash        *sh_page_manager__new_hash_fullpage(  sh_page_manager *self ); // create a hash giving it a page of its own
sh_bighash     *sh_page_manager__new_bighash(        sh_page_manager *self, u4 desired_size ); // create a new "big hash"
sh_page        *sh_page_manager__getpage(            sh_page_manager *self );
sh_page        *sh_page_manager__getpage_by_pagenum( sh_page_manager *self, PAGE_ID_TYPE pagenum );
sh_page        *sh_page_manager__next_emptyish_page( sh_page_manager *self );
int             sh_page_manager__store_z(            sh_page_manager *self, sh_hash *hashob   , char *name, STORED_TYPE value );
int             sh_page_manager__store(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen, STORED_TYPE value );
int             sh_page_manager__store_u1(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen, u1 value );
int             sh_page_manager__store_u2(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen, u2 value );
int             sh_page_manager__store_u4(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen, u4 value );
int             sh_page_manager__store_str(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen, char *str, uint16_t strlen );
int             sh_page_manager__store_bighash(      sh_page_manager *self, sh_bighash *hashob, char *name, u1 namelen, STORED_TYPE value );
STORED_TYPE    *sh_page_manager__fetch_z(            sh_page_manager *self, sh_hash *hashob   , char *name );
STORED_TYPE    *sh_page_manager__fetch(              sh_page_manager *self, sh_hash *hashob   , char *name, u1 namelen );
STORED_TYPE    *sh_page_manager__fetch_bighash(      sh_page_manager *self, sh_bighash *hashob, char *name, u1 namelen );
void            sh_page_manager__debug(              sh_page_manager *self );

#endif