// Copyright (C) 2014 David Helkowski
// License GNU AGPLv3

#ifndef SH_HASH_FUNC_H
#define SH_HASH_FUNC_H

#include<stdint.h>
#include"sh_shared.h"

uint32_t hashstr_z(     const char *str, u4 seed );
uint32_t hashstr(       const char *str, const u1 strlen, u4 seed );
uint32_t hashstr_mul_z( const char *str, u2 mul, u4 seed );
uint32_t hashstr_mul(   const char *str, const u1 strlen, u2 mul, u4 seed );

#endif