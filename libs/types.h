#ifndef TYPES
#define TYPES

#define or ||
#define nullptr (void*)0
#define not !
#define and &&

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef unsigned int uint;
typedef size_t usz;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int32_t i64;

typedef float f32;
typedef double f64;

struct _location {
  uint col;
  uint row;
};

struct _span {
  uint col;
  uint row;
  uint len;
};

struct _string {
  char* ptr;
  size_t len;
};

struct _dynamic_string {
  char* ptr;
  size_t len;
};

typedef union {
  struct _dynamic_string dynamic;
  struct _string compile_time;
  struct {
    char* ptr;
    size_t len;
  };
} __attribute__((transparent_union)) any_string;

#define string(ptr) (struct _string){ ptr, sizeof(ptr) / sizeof(ptr[0]) }
#define dynamic_string(ptr) (struct _dynamic_string){ ptr, strlen(ptr) }

typedef struct _string string;
typedef struct _span span;
typedef struct _location location;
typedef struct _dynamic_string dynamic_string;

#endif // !TYPES
