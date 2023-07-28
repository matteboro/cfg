#pragma once 

#include <stdint.h>

#undef TODO
#undef UNREACHABLE
#define UNREACHABLE() assert(False && "UNREACHABLE\n")
#define TODO() assert(False && "TODO\n")

typedef uint64_t ByteSize;
typedef uint64_t MemTableIndex;
typedef uint64_t Offset;

typedef uint64_t ArraySize;
typedef uint64_t ArrayIndex;
typedef struct Object_d Object;

const static MemTableIndex NullMemTableIndex = 0;
