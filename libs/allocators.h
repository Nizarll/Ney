#ifndef ALLOCATORS
#define ALLOCATORS

#include "types.h"

typedef void* (*allocator_func)(usz, void*);

struct _allocator {
  void* ctx;
  allocator_func callback;
};

struct _arena {
  u8* data;
  uint occupied;
  uint total;
};

struct _arena_list {
  struct _arena* arena_list;
  uint current;
  uint size;
};

typedef struct _arena arena;
typedef struct _arena_list arena_list;
typedef struct _allocator allocator;

arena arena_init(uint size);
arena arena_deinit(arena* arena);
arena_list arena_list_init(uint arena_size, uint num_arenas);
void arena_list_deinit(arena_list* pool);

allocator arena_allocator_init(arena* arena);
allocator arena_list_allocator_init(arena_list* list); //TODO: unimplemented

__always_inline void*  allocator_alloc(allocator* alloc, size_t size) { return alloc->callback(size, alloc->ctx); }

#endif // !allocators
