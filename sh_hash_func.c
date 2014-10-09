// Copyright (C) 2014 David Helkowski
// License GNU AGPLv3

#include"sh_shared.h"
#include"sh_hash_func.h"
#include<stdio.h> 
#include<time.h>
#include<string.h>
#include<stdint.h>

#define BYTE_MULT 257

u4 hashstr_z( const char *str, u4 seed ) {
  u4 sum = seed;//4,294,967,295 / 257 = ... ( nearest prime )
  const char *pos = str;
  u1 i;
  while( 1 ) {
    if( ! *pos ) break;
    sum += *pos;
    if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16,711,678 is max
    sum *= BYTE_MULT; // prime
    pos++;
  }
  if( (pos - str) < 6 ) { // ensure seed is scrambled some
    for( i=0;i<( 6 - (pos - str) );i++ ) {
      sum += 137; // prime
      if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16711678 is max
      sum *= BYTE_MULT; // multi by 0xFF -> shift left one; also prime ( 241 ( 255 - 16 ) should function similarly )
    }
  }
  return sum;
}

u4 hashstr( const char *str, const u1 namelen, u4 seed ) {
  u4 sum = seed;//4,294,967,295 / 257 = ... ( nearest prime )
  const char *pos = str;
  u1 i;
  for( i = 0; i < namelen; i++ ) {
    sum += *pos;
    if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16,711,678 is max
    sum *= BYTE_MULT; // prime
    pos++;
    i++;
  }
  if( namelen < 6 ) { // ensure seed is scrambled some
    for( i=0; i < ( 6 - namelen ); i++ ) {
      sum += 137; // prime
      if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16711678 is max
      sum *= BYTE_MULT; // prime
    }
  }
  return sum;
}

u4 hashstr_mul_z( const char *str, u2 mul, u4 seed ) {
  u4 sum = seed;//4,294,967,295 / 257 = ... ( nearest prime )
  const char *pos = str;
  u1 i;
  while( 1 ) {
    if( ! *pos ) break;
    sum += *pos;
    if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16,711,678 is max
    sum *= mul; // prime
    pos++;
  }
  if( (pos - str) < 6 ) { // ensure seed is scrambled some
    for( i=0;i<( 6 - (pos - str) );i++ ) {
      sum += 137; // prime
      if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16711678 is max
      sum *= mul; // multi by 0xFF -> shift left one; also prime ( 241 ( 255 - 16 ) should function similarly )
    }
  }
  return sum;
}

u4 hashstr_mul( const char *str, const u1 namelen, u2 mul, u4 seed ) {
  u4 sum = seed;//4,294,967,295 / 257 = ... ( nearest prime )
  const char *pos = str;
  u1 i;
  for( i = 0; i < namelen; i++ ) {
    sum += *pos;
    if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16,711,678 is max
    sum *= mul; // prime
    pos++;
    i++;
  }
  if( namelen < 6 ) { // ensure seed is scrambled some
    for( i=0; i < ( 6 - namelen ); i++ ) {
      sum += 137; // prime
      if( sum > 16711661 ) sum -= 16711661; // prime - prevent overflow - 16711678 is max
      sum *= mul; // prime
    }
  }
  return sum;
}