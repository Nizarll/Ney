#ifndef ALLOC_H
#define ALLOC_H

#include "error.h"
#include <stdlib.h>

#define U16_MAX    0xffff

#define type_cast(PTR, TYPE) ((TYPE)(PTR))

struct hybrd_b { // hybrid buffer
  size_t size;
  size_t curr;
  size_t elem_size;
  struct {
    char s_buff[U16_MAX];
    heap_b* hp;
  };
};

struct heap_b { // heap buffer
  void* buff;
  size_t curr;
  size_t size;
  size_t elem_size;
  size_t grow_size;
};

static struct reg_buffs {
  uint16_t curr; // this might lead to some memory not being freed
  // if the amount of buffer exceeds 65365
  // but if you use such an amount of buffers then something is wrong ?
  heap_b buffs[U16_MAX];
} reg_buffs = {0};

typedef struct stack_b stack_b;
typedef struct heap_b heap_b;

heap_b* heap_new(heap_b heap);
void heap_grow(heap_b* h, size_t size);
void heap_destroy(heap_b* h);

#endif
