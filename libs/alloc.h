#ifndef ALLOC_H
#define ALLOC_H

#include "error.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define U16_MAX 0xffff

#define cast(PTR, TYPE) (TYPE PTR)

struct hybrd_b { // hybrid buffer
  size_t curr;
  size_t elem_size;
  struct {
    char s_buff[U16_MAX];
    struct heap_b* hp;
  };
};

struct heap_b { // heap buffer
  void* buff;
  size_t curr;
  size_t size;
  size_t elem_size;
  size_t grow_size;
};

static struct {
  uint16_t curr; // this might lead to some memory not being freed
  // if the amount of buffer exceeds 65365
  // but if you use such an amount of buffers then something is wrong ?
  struct heap_b* buffs[U16_MAX];
} reg_buffs = {0};

typedef struct hybrd_b hybrd_b;
typedef struct heap_b heap_b;

heap_b* heap_new(heap_b heap);
void heap_grow(heap_b* h);
void destroy_heaps();
void* heap_b_append(heap_b* h, void* elem);
void* hybrd_b_append(hybrd_b* b, void* elem);

#endif
