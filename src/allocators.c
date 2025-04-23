#include "allocators.h"
#include "log.h"

void* arena_alloc(usz size, void* ctx)
{
  arena* arena = ctx;
#ifdef debug
  if (arena == nullptr)
    ney_err("attempted to allocate from nullptr arena");
#endif

  if (arena->occupied + size >= arena->total) {
    return nullptr;
  }
  
  u8* ptr = arena->data + arena->occupied;
  arena->occupied += size;
  return ptr;
}

arena arena_init(usz size)
{
  void* buffer = malloc(size);

  if (!buffer)
    ney_err("arena_init: failure");

  return (arena){
    buffer,
    0,
    size
  };
}

void arena_deinit(arena* arena)
{
  free(arena->data);
  arena->data = nullptr;
}

arena_list arena_list_init(uint arena_size, uint num_arenas)
{
  arena* buffer = malloc(num_arenas * sizeof(arena));
  
  if (buffer == nullptr)
    ney_err("Heap overflow: failed to initialize arena_list consider getting more ram...");
  
  for (int i = 0; i < num_arenas; i++)
    buffer[i] = arena_init(arena_size);
  
  return (arena_list){
    .arena_list = buffer,
    .current = 0,
    .size = num_arenas
  };
}

allocator arena_allocator_init(arena* arena)
{
  return (allocator) {
    .ctx = (void*)arena,
    .allocate = arena_alloc
  };
}
