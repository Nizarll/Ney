#ifndef ALLOCATORS
#define ALLOCATORS

#include "utils.h"

typedef void *(*allocator_func)(usz, void *);

struct _allocator {
  void *ctx;
  allocator_func allocate;
};

struct _arena {
  u8 *data;
  uint occupied;
  uint total;
};

struct _arena_list {
  struct _arena *arena_list;
  uint current;
  uint size;
};

typedef struct _allocator allocator;
typedef struct _arena arena;
typedef struct _arena_list arena_list;

arena arena_init(usz size);
allocator arena_allocator_init(arena* arena);
void __always_inline *allocator_alloc(allocator *alloc, size_t size) { return alloc->allocate(size, alloc->ctx); }

#endif // !allocators
