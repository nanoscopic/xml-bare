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
  hash      *basehash = xb( parse   , man, "<root><x>4</x><abc/></root>" );
  hash      *roothash = xb( get_hash, man, basehash, "root", 4 );
  hash      *xhash    = xb( get_hash, man, roothash, "x", 1 );
  str_w_len *strn     = xb( get_val , man, xhash );
  
  printf("Contains: %.*s\n", strn->len, strn->str );
}