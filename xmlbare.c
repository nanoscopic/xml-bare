// JEdit mode Line -> :folding=indent:mode=c++:indentSize=2:noTabs=true:tabSize=2:
#include"parser.h"
#include"xmlbare.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"shared_hashing.h"

typedef sh_page_manager pageman;
typedef sh_bighash bighash;
typedef sh_hash hash;
#define pageman( a, ... ) sh_page_manager__ ## a( __VA_ARGS__ )

//#define DEBUG

sh_page_manager  *xmlbare__new_pageman() {
  return sh_page_manager__new();
}

sh_hash *xmlbare__parse( sh_page_manager *man, char *xmlsrc ) {
  struct parserc *parser = xbp( new );
  xbp( parse, parser, xmlsrc );
  xmlnode *base = xbp( xml2obj, parser, man, parser->rootnode );
  sh_hash *basehash = ( sh_hash * ) base->ptr;
  return basehash;
}

sh_hash *xmlbare__get_hash( sh_page_manager *man, sh_hash *hash, char *str, u2 strlen ) {
   xmlnode **nodeptr = (xmlnode **) pageman( fetch, man, hash, str, strlen );
   if( !nodeptr ) return NULL;
   xmlnode *node = *nodeptr;
   if( node->type != XNODE_HASH ) return NULL; 
   return ( sh_hash * ) node->ptr;
}

xmlnode_array *xmlbare__get_arr( sh_page_manager *man, sh_hash *hash, char *str, u2 strlen ) {
   xmlnode **nodeptr = (xmlnode **) pageman( fetch, man, hash, str, strlen );
   if( !nodeptr ) return NULL;
   xmlnode *node = *nodeptr;
   if( node->type != XNODE_ARR ) return NULL; 
   return ( xmlnode_array * ) node->ptr;
}

string_with_len * xmlbare__get_val( sh_page_manager *man, sh_hash *hash ) {
    string_with_len **nodeptr = (string_with_len **) pageman( fetch, man, hash, "_value", 6 );
    if( !nodeptr ) return NULL;
    string_with_len *node = *nodeptr;
    return node;
}

xmlnode_array *xmlbare_array__new() {
  xmlnode_array *array = newmalloc( xmlnode_array );
  array->type = XNODE_ARR;
  array->count = 0;
  array->root = NULL;
  return array;
}

void xmlbare_array__push( xmlnode_array *array, xmlnode *item ) {
  xmlnode_array_node *node = newmalloc( xmlnode_array_node );
  node->node = item;
  if( !array->count ) {
    node->next = NULL;
  }
  else {
    node->next = array->root;
  }
  array->root = node;
  array->count++;
}

xmlnode *xmlbare_parser__xml2obj( struct parserc *parser, sh_page_manager *man, struct nodec *curnode ) {
  hash *output = pageman( new_hash, man ); // the root
  int i; // loop index; defined at the top because this is C
  struct attc *curatt; // current attribute being worked with
  int numatts = curnode->numatt; // total number of attributes on the current node
  int cur_type;
  int length = curnode->numchildren;
  
  pageman( store_u2, man, output, "_pos", 4, curnode->pos );
  pageman( store_u2, man, output, "_i"  , 2, curnode->name - rootpos );
  pageman( store_u2, man, output, "_z"  , 2, curnode->z );
  
  #ifdef DEBUG
  printf("Node: %.*s\n", curnode->namelen, curnode->name );
  #endif
  
  // node without children
  if( !length ) {
    if( curnode->vallen ) {
      pageman( store_str, man, output, "_value", 6, curnode->value, curnode->vallen );
      if( curnode->type ) pageman( store_u1, man, output, "_cdata", 6, 1 );
    }
    if( curnode->comlen ) pageman( store_str, man, output, "_comment", 8, curnode->comment, curnode->comlen );
  }
  
  // node with children
  else {
    if( curnode->vallen ) {
      pageman( store_str, man, output, "_value", 6, curnode->value, curnode->vallen );
      if( curnode->type ) pageman( store_u1, man, output, "_cdata", 6, 1 );
    }
    if( curnode->comlen ) pageman( store_str, man, output, "_comment", 8, curnode->comment, curnode->comlen );
    
    // loop through child nodes
    curnode = curnode->firstchild;
    for( i = 0; i < length; i++ ) {
      xmlnode **cur = (xmlnode **) pageman( fetch, man, output, curnode->name, curnode->namelen );
      
      // check for multi_[name] nodes
      if( curnode->namelen > 6 ) {
        if( !strncmp( curnode->name, "multi_", 6 ) ) {
          char *subname = &curnode->name[6];
          int subnamelen = curnode->namelen-6;
          xmlnode **old = (xmlnode **) pageman( fetch, man, output, subname, subnamelen );
          xmlnode_array *newarray = xba( new );
          if( !old ) pageman( store, man, output, subname, subnamelen, (void *) newarray );
          else if( (*old)->type == XNODE_HASH ) { // check for hash ref
            //pageman( del, man, output, subname, subnamelen );
            pageman( store, man, output, subname, subnamelen, (void *) newarray );
            xba( push, newarray, *old );
          }
        }
      }
      
      if( !cur ) {
        xmlnode *ob = xbp( xml2obj, parser, man, curnode );
        pageman( store, man, output, curnode->name, curnode->namelen, ob );
      }
      else { // there is already a node stored with this name
        cur_type = (*cur)->type;
        if( cur_type == XNODE_HASH ) { // sub value is a hash; must be anode
          xmlnode_array *newarray = xba( new );
          //pageman( del, man, output, curnode->name, curnode->namelen );
          pageman( store, man, output, curnode->name, curnode->namelen, (void *) newarray );
          xba( push, newarray, *cur );
          xmlnode *ob = xbp( xml2obj, parser, man, curnode );
          xba( push, newarray, ob );
        }
        else if( cur_type == XNODE_ARR ) {
          xmlnode *ob = xbp( xml2obj, parser, man, curnode );
          xba( push, ( xmlnode_array *) cur, ob );
        }
        else {
          // something else; probably an existing value node; just wipe it out
          xmlnode *ob = xbp( xml2obj, parser, man, curnode );
          pageman( store, man, output, curnode->name, curnode->namelen, ob );
        }
      }
      if( i != ( length - 1 ) ) curnode = curnode->next;
    }
    
    curnode = curnode->parent;
  }
  
  if( numatts ) {
    curatt = curnode->firstatt;
    for( i = 0; i < numatts; i++ ) {
      hash *atth = pageman( new_hash, man );
      pageman( store, man, output, curatt->name, curatt->namelen, atth );
      
      char *attval;
      uint16_t attlen;
      if( curatt->value == -1 ) { attval = "1";           attlen = 1;              }
      else                      { attval = curatt->value; attlen = curatt->vallen; }
      pageman( store_str, man, atth, "_value", 6, attval, attlen );
      pageman( store_u1, man, atth, "_att", 4, 1 );
      if( i != ( numatts - 1 ) ) curatt = curatt->next;
    }
  }
  
  xmlnode *ret = (xmlnode *) malloc( sizeof( xmlnode ) );
  ret->type = XNODE_HASH;
  ret->ptr = output;
  return ret;
}

//xmlnode *xml2obj( struct parserc *parser ) {
//  if( parser->err ) return 0;//parser->err;
//  else return cxml2obj( parser, parser->rootnode );
//}
    
int xmlbare_parser__parse_more( struct parserc *parser, char *text ) {
  int err = parserc_parse( parser, text );
  return err;
}

struct parserc *xmlbare_parser__new() {
  struct parserc *parser = (struct parserc *) malloc( sizeof( struct parserc ) );
  return parser;
}

int xmlbare_parser__parse( struct parserc *parser, char *text ) {
  parser->last_state = 0;
  int err = parserc_parse( parser, text );
  return err;
}

int xmlbare_parser__parse_unsafely( struct parserc *parser, char *text ) {
  parser->last_state = 0;
  int err = parserc_parse_unsafely( parser, text );
  return err;
}

int xmlbare_parser__parse_file( struct parserc *parser, char *filename ) {
  char *data;
  unsigned long len;
  FILE *handle;
  
  handle = fopen(filename,"r");
  
  fseek( handle, 0, SEEK_END );
  
  len = ftell( handle );
  
  fseek( handle, 0, SEEK_SET );
  data = (char *) malloc( len );
  rootpos = data;
  fread( data, 1, len, handle );
  fclose( handle );
  parser->last_state = 0;
  int err = parserc_parse( parser, data );
  return err;
}

void xmlbare_parser__del( struct parserc *parser ) {
  struct nodec *rootnode = parser->rootnode;
  del_nodec( rootnode ); // note this frees the pointer as well
  free( parser );
}