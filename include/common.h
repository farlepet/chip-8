#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#ifndef COMMON_H
#define COMMON_H

#define DEBUG

#ifdef DEBUG
#define D printf
#define FLS() fflush(stdout)
#else
#define D(str, ...)
#endif

typedef unsigned char      u8int;
typedef unsigned short     u16int;
typedef unsigned long      u32int;
typedef unsigned long long u64int;

typedef char      s8int;
typedef short     s16int;
typedef long      s32int;
typedef long long s64int;

#endif