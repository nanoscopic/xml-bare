#ifndef __XML_BARE_H
#define __XML_BARE_H
#include"parser.h"
#include<stdint.h>
#include"sh_page_manager.h"

char *rootpos;

//#define DEBUG
#define newmalloc( type ) ( type * ) malloc( sizeof( type ) )

#define XNODE_HASH 1 // a hash
#define XNODE_ARR  2 // an array of nodes
#define XNODE_TYPED_ARR 3 // an array of things of one type
#define XNODE_STR  4
#define XNODE_STRZ 5
#define XNODE_U8   6
#define XNODE_U16  7
#define XNODE_U32  8

struct xmlnode_s {
  uint8_t type;
  void *ptr;
};
typedef struct xmlnode_s xmlnode;

struct xmlnode_array_node_s {
  xmlnode *node;
  struct xmlnode_array_node_s *next;
};
typedef struct xmlnode_array_node_s xmlnode_array_node;

struct xmlnode_array_s {
  uint8_t type;
  uint16_t count;
  xmlnode_array_node *root;
};
typedef struct xmlnode_array_s xmlnode_array;

#define xb( a, ... ) xmlbare__ ## a( __VA_ARGS__ )
#define xba( a, ... ) xmlbare_array__ ## a( __VA_ARGS__ )
#define xbp( a, ... ) xmlbare_parser__ ## a( __VA_ARGS__ )

sh_page_manager  *xmlbare__new_pageman();
sh_hash          *xmlbare__parse(    sh_page_manager *man, char *xmlsrc );
sh_hash          *xmlbare__get_hash( sh_page_manager *man, sh_hash *hash, char *str, u2 strlen );
xmlnode_array    *xmlbare__get_arr(  sh_page_manager *man, sh_hash *hash, char *str, u2 strlen );
string_with_len  *xmlbare__get_val(  sh_page_manager *man, sh_hash *hash );

xmlnode_array    *xmlbare_array__new();
void              xmlbare_array__push( xmlnode_array *array, xmlnode *item );

struct parserc   *xmlbare_parser__new();
xmlnode          *xmlbare_parser__xml2obj(        struct parserc *parser, sh_page_manager *man, struct nodec *curnode );
int               xmlbare_parser__parse_more(     struct parserc *parser, char *text );
int               xmlbare_parser__parse(          struct parserc *parser, char *text );
int               xmlbare_parser__parse_unsafely( struct parserc *parser, char *text );
int               xmlbare_parser__parse_file(     struct parserc *parser, char *filename );
void              xmlbare_parser__del(            struct parserc *parser );
#endif