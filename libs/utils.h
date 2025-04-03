#ifndef UTILS
#define UTILS

#define or ||
#define nullptr (void*)0
#define not !
#define and &&

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

//NOTE: dynamic arrays / vector macros:
// requires the dynamic array to have the members:
// type* items;
// usz occupied;
// usz total;

#define LIST_BASE_SIZE 32
#define LIST_REALLOC_RATIO 1.5

#define LIST_INIT(list, type)                                      \
{                                                                   \
  list->items = ney_alloc(LIST_BASE_SIZE * sizeof(type));            \
  list->occupied = 0;                                                 \
  list->total = LIST_BASE_SIZE;                                        \
  if (list->items == nullptr) ney_err("list allocation failure");       \
}

#define LIST_PUSH(list, item)                                              \
{                                                                           \
  if (list->items == nullptr) {                                              \
    LIST_INIT(list, typeof(item));                                            \
  } else if (list->occupied >= list->total) {                                  \
    list->total = list->total * LIST_REALLOC_RATIO;                             \
    list->items = ney_realloc(list->tokens, list->total * sizeof(token));        \
    if (list->items == nullptr) ney_err("tokenlist allocation failure");          \
  }                                                                                \
  memcpy(list->items + list->occupied++, &list, sizeof(token));                     \
}

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

#define string(ptr) (struct _string){ ptr, (sizeof(ptr) / sizeof(ptr[0])) }
#define dynamic_string(ptr) (struct _dynamic_string){ ptr, strlen(ptr) }

typedef struct _string string;
typedef struct _span span;
typedef struct _location location;
typedef struct _dynamic_string dynamic_string;

#endif // !UTILS
