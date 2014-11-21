// Copyright (C) 2014 David Helkowski
// License GNU AGPLv3

#include"shared_hashing.h"
#include"sh_shared.h"
#include"xmlbare.h"
#include<stdio.h>
#include<stdlib.h>

typedef sh_page_manager pageman;
typedef sh_hash hash;
typedef string_with_len str_w_len;

int main( int argc, char *argv[] ) {
  pageman   *man      = xb( new_pageman );
  hash      *basehash = xb( parse   , man, "<root><x>4</x><b>1</b><b>2</b><abc/></root>" );
  hash      *roothash = xb( get_hash, man, basehash, "root", 4 );
  hash      *xhash    = xb( get_hash, man, roothash, "x", 1 );
  str_w_len *strn     = xb( get_val , man, xhash );
  
  printf("Contains: %.*s\n", strn->len, strn->str );
  
  xmlnode_array *res = xb( get_arr, man, roothash, "b", 1 );
  if( res == NULL ) {
      printf("Error 1\n");
  }
  else {
      printf("Roothash is %p\n", roothash );
      xmlnode_array_node *first = res->root;
      printf("Array is %p - array node 1 is %p, xmlnode is %p, hash is %p\n", res, first, first->node, first->node->ptr );
      xmlnode *node = first->node;
      string_with_len *strplus = xb( get_val, man, ( hash * ) node->ptr );
      printf("Value of first b: %.*s\n", strplus->len, strplus->str );
      //xmlnode *next = first->next;
      xmlnode_array_node *next = first->next;
      xmlnode *nextnode = next->node;
      if( nextnode == NULL ) {
          printf("Next is NULL\n");
      }
      else {
          hash *hash2 = ( hash * ) nextnode->ptr;
          printf("Array is %p - array node 2 is %p, xmlnode is %p, hash is %p\n", res, next, nextnode, hash2 );
          //printf("Hash2 is %p\n", hash2 );
          string_with_len *strnext = xb( get_val, man, hash2 );
          printf("Value of second b: %.*s\n", strnext->len, strnext->str );
      }
  }
  
  return 0;
}